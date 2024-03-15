#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <unordered_map>
#include <shared_mutex>
#include <chrono>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Service.h"
#include "Conn.h"
using namespace std;

class ThreadPool
{
private:
    const unsigned WORKER_NUM;
    vector<thread> workerThreads;

    mutex stop_mtx;
    bool stop{false};

private:
    ThreadPool(unsigned num = thread::hardware_concurrency())
        : WORKER_NUM(num)
    {
        // 创建epoll
        epollFd = epoll_create(1024); // 返回值：非负数:成功的描述符，-1失败
        assert(epollFd > 0);
    }

public:
    static ThreadPool *instance()
    {
        static ThreadPool pool;
        return &pool;
    }

private:
    // 服务列表
    unordered_map<unsigned, shared_ptr<Service>> services;
    unsigned max_id{0};        // 最大ID
    shared_mutex services_mtx; // 读写锁

    // 全局队列
    queue<shared_ptr<Service>> global_queue;
    unsigned global_len{0}; // 队列长度
    mutex global_mtx;       // 锁

    // 休眠和唤醒
    mutex sleep_mtx;
    condition_variable sleep_cv;
    unsigned sleep_count{0}; // 休眠工作线程数
    
    // Conn列表
    unordered_map<unsigned, shared_ptr<Conn>> conns;
    shared_mutex connsLock; // 读写锁

    // epoll描述符
    int epollFd; 

public:
    // 增加服务
    unsigned AddService(const string &type)
    {
        shared_ptr<Service> srv(new Service());
        srv->type = type;

        unique_lock<shared_mutex> lock(services_mtx);
        srv->id = max_id++;
        services.emplace(srv->id, srv);
        lock.unlock();

        srv->OnInit(); // 初始化
        return srv->id;
    }
    // 移除服务
    void RemoveService(unsigned id)
    {
        shared_ptr<Service> srv = GetService(id);
        if (!srv)
            return;

        srv->isExiting = true;
        srv->OnExit();

        unique_lock<shared_mutex> lock(services_mtx);
        services.erase(id);
    }
    // 发送消息
    void Send(shared_ptr<BaseMsg> msg)
    {
        unsigned toId = msg->to;
        shared_ptr<Service> toSrv = GetService(toId);
        if (!toSrv)
        {
            cout << "Send fail, toSrv not exist toId: " << toId << endl;
            return;
        }
        toSrv->PushMsg(msg);

        bool hasPush = false;
        toSrv->inGlobal_mtx.lock();
        if (!toSrv->inGlobal)
        {
            PushGlobalQueue(toSrv);
            toSrv->inGlobal = true;
            hasPush = true;
        }
        toSrv->inGlobal_mtx.unlock();

        if (hasPush)
            CheckAndWeakUp();
    }

    // 全局队列操作
    shared_ptr<Service> PopGlobalQueue()
    {
        shared_ptr<Service> srv = nullptr;
        global_mtx.lock();
        if (!global_queue.empty())
        {
            srv = global_queue.front();
            global_queue.pop();
            global_len--;
        }
        global_mtx.unlock();
        return srv;
    }
    void PushGlobalQueue(shared_ptr<Service> srv)
    {
        global_mtx.lock();
        global_queue.push(srv);
        global_len++;
        global_mtx.unlock();
    }

    // 工作线程等待
    void WorkerWait()
    {
        sleep_mtx.lock();
        sleep_count++;
        sleep_mtx.unlock();

        unique_lock<mutex> lock(sleep_mtx);
        sleep_cv.wait(lock);
        sleep_count--;
    }

private:
    shared_ptr<Service> GetService(unsigned id)
    {
        shared_ptr<Service> srv = nullptr;
        shared_lock<shared_mutex> lock(services_mtx);
        unordered_map<unsigned, shared_ptr<Service>>::const_iterator iter = services.find(id);
        if (iter != services.cend())
            srv = iter->second;
        lock.unlock();
        return srv;
    }

    void WeakUp()
    {
        cout << "weakup" << endl;
        sleep_cv.notify_all();
    }

    void CheckAndWeakUp()
    {
        if (sleep_count != 0 && WORKER_NUM - sleep_count <= global_len)
            WeakUp();
    }

    void CheckAndPutGlobal(shared_ptr<Service> srv)
    {
        if (srv->isExiting)
            return;

        srv->msg_mtx.lock();
        if (!srv->msg_queue.empty()) // 重新放回全局队列
            PushGlobalQueue(srv);    // 此时srv->inGlobal一定是true
        else
            srv->SetInGlobal(false); // 不在队列中，重设inGlobal
        srv->msg_mtx.unlock();
    }

public:
    void Start()
    {
        cout << "Enter ThreadPool" << endl;
        for (unsigned i = 0; i < WORKER_NUM; ++i)
        {
            cout << "start worker thread: " << i << endl;
            workerThreads.emplace_back([this]{
                while (true)
                {
                    {
                    lock_guard<mutex> lock(stop_mtx);
                        if(stop)
                            return;
                    }

                    shared_ptr<Service> srv = PopGlobalQueue();
                    if (!srv)
                    {
                        WorkerWait();
                    }
                    else
                    {
                        srv->ProcessMsg();
                        CheckAndPutGlobal(srv);
                    }
                } 
            });
        }
        cout << "start socket thread"  << endl;
        workerThreads.emplace_back([this]{
            cout << "SocketWorker working"  << endl;
                
            const int EVENT_SIZE = 64;
            struct epoll_event events[EVENT_SIZE];
            while (true)
            {
                int eventCount = epoll_wait(epollFd, events, EVENT_SIZE, -1);// 阻塞等待
                for (int i = 0; i < eventCount; i++)// 取得事件
                    OnEvent(events[i]);// 当前要处理的事件
            }
        });
    }
    void Wait()
    {
        stop_mtx.lock();
        stop = true;
        stop_mtx.unlock();
        WeakUp();

        for (thread &worker : workerThreads)
            worker.join();
        workerThreads.clear();
        cout << "Leave ThreadPool" << endl;
    }
public:// 增删查Conn
    int AddConn(unsigned fd, unsigned id, Conn::TYPE type)
    {
        shared_ptr<Conn> conn(new Conn());
        conn->fd = fd;
        conn->serviceId = id;
        conn->type = type;
        lock_guard<shared_mutex> lock(connsLock);
        conns.emplace(fd, conn);
        return fd;
    }
    shared_ptr<Conn> GetConn(unsigned fd)
    {
        shared_ptr<Conn> conn = nullptr;
        shared_lock<shared_mutex> lock(connsLock);
        unordered_map<unsigned, shared_ptr<Conn>>::const_iterator iter = conns.find (fd);
        if (iter != conns.cend())
            conn = iter->second;
        return conn;
    }
    bool RemoveConn(unsigned fd)
    {
        shared_lock<shared_mutex> lock(connsLock);
        return 1 == conns.erase(fd);
    }

public:
    void AddEvent(int fd)
    {
        cout << "AddEvent fd " << fd << endl;
        
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = fd;

        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == -1)
            cout << "AddEvent epoll_ctl Fail:" << strerror(errno) << endl;
    }
    void RemoveEvent(int fd)
    {
        cout << "RemoveEvent fd " << fd << endl;
        epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
    }
    void ModifyEvent(int fd, bool epollOut)
    {
        cout << "ModifyEvent fd " << fd << " " << epollOut << endl;
        struct epoll_event ev;
        ev.data.fd = fd;

        if (epollOut)
            ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
        else
            ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev);
    }

private:
    void OnRW(shared_ptr<Conn> conn, bool r, bool w)
    {
        cout << "OnRW fd:" << conn->fd << endl;
        shared_ptr<SocketRWMsg> msg(new SocketRWMsg());
        msg->from = conn->fd;
        msg->to = 1;
        msg->buff = "read or write message";
        msg->fd = conn->fd;
        msg->isRead = r;
        msg->isWrite = w;
        ThreadPool::instance()->Send(msg);
    }   
    void OnEvent(epoll_event ev)
    {
        int fd = ev.data.fd;
        auto conn = ThreadPool::instance()->GetConn(fd);
        if (conn == nullptr)
        {
            cout << "OnEvent error, conn == NULL" << endl;
            return;
        }
        // 事件类型
        bool isRead = ev.events & EPOLLIN;
        bool isWrite = ev.events & EPOLLOUT;
        bool isError = ev.events & EPOLLERR;
        // 监听Socket
        if (conn->type == Conn::TYPE::LISTEN)
        {
            if (isRead)
                OnAccept(conn);
        }
        // 普通Socket
        else
        {
            if (isRead || isWrite)
                OnRW(conn, isRead, isWrite);

            if (isError)
                cout << "OnError fd:" << conn->fd << endl;
        }
    }
    void OnAccept(shared_ptr<Conn> conn)
    {
        cout << "OnAccept fd:" << conn->fd << endl;
        // 步骤1：accept
        int clientFd = accept(conn->fd, NULL, NULL);
        if (clientFd < 0)
            cout << "accept error" << endl;

        // 步骤2：设置非阻塞
        fcntl(clientFd, F_SETFL, O_NONBLOCK);

        // 写缓冲区大小
        // unsigned long buffSize = 4294967295;
        // if(setsockopt(clientFd, SOL_SOCKET, SO_SNDBUFFORCE , &buffSize, sizeof(buffSize)) < 0)
        //     cout << "OnAccept setsockopt Fail " << strerror(errno) << endl;

        // 步骤3：添加到管理结构
        ThreadPool::instance()->AddConn(clientFd, conn->serviceId, Conn::TYPE::CLIENT);

        // 步骤4：添加到epoll
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = clientFd;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1)
            cout << "OnAccept epoll_ctl Fail:" << strerror(errno) << endl;

        // 步骤5：通知
        shared_ptr<SocketAcceptMsg> msg(new SocketAcceptMsg());
        msg->from = conn->fd;
        msg->to = conn->serviceId;
        msg->buff = "read or write message";
        msg->listenFd = conn->fd;
        msg->clientFd = clientFd;
        ThreadPool::instance()->Send(msg);
    }
public:
    // 网络连接操作接口（用原始read write）
    int Listen(unsigned port, unsigned serviceId)
    {
        //创建Socket
        int listenFd = socket(AF_INET, SOCK_STREAM, 0);
        if(listenFd <= 0){
            cout << "listen error, listenFd <= 0" << endl;
            return -1;
        }
        fcntl(listenFd, F_SETFL, O_NONBLOCK);

        //创建地址结构
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        //bind
        int r = bind(listenFd, (struct sockaddr*)&addr, sizeof(addr));
        if( r == -1){
            cout << "listen error, bind fail" << endl;
            return -1;
        }

        //listen
        r = listen(listenFd, 64); //see
        if(r < 0)
            return -1;
        
        //添加到管理结构
        AddConn(listenFd, serviceId, Conn::TYPE::LISTEN);

        //Epoll事件（跨线程）
        AddEvent(listenFd);
        return listenFd;
    }
    void CloseConn(unsigned fd)
    {
        //删除管理结构
        bool succ = RemoveConn(fd);

        //关闭
        close(fd);

        //Epoll事件（跨线程）
        if(succ) 
            RemoveEvent(fd);
    }
};
