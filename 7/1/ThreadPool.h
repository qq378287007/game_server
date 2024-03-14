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
#include "Service.h"
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
};
