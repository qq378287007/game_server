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
using namespace std;

#include "Service.h"

class ThreadPool
{
    const size_t WORKER_NUM;
    vector<thread> workers;

    mutex stop_mtx;
    bool stop{false};

private:
    // 服务列表
    unordered_map<uint32_t, shared_ptr<Service>> services;
    uint32_t max_id{0};        // 最大ID
    shared_mutex services_mtx; // 读写锁

    // 全局队列
    queue<shared_ptr<Service>> global_queue;
    int global_len{0}; // 队列长度
    mutex global_mtx;  // 锁

    // 休眠和唤醒
    mutex sleep_mtx;
    condition_variable sleep_cv;
    int sleep_count{0}; // 休眠工作线程数

public:
    ThreadPool(size_t num = thread::hardware_concurrency())
        : WORKER_NUM(num)
    {
        cout << "Enter ThreadPool" << endl;

        for (size_t i = 0; i < WORKER_NUM; ++i)
        {
            workers.emplace_back([this]
                                 {
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
                    } });
        }
    }
    ~ThreadPool()
    {
        stop_mtx.lock();
        stop = true;
        stop_mtx.unlock();

        sleep_mtx.lock();
        sleep_count = WORKER_NUM;
        sleep_mtx.unlock();
        sleep_cv.notify_all();

        for (thread &worker : workers)
            worker.join();

        cout << "Leave ThreadPool" << endl;
    }

    // 增删服务
    uint32_t AddService(shared_ptr<string> type)
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

    void RemoveService(uint32_t id) // 仅限服务自己调用
    {
        shared_ptr<Service> srv = GetService(id);
        if (!srv)
            return;

        unique_lock<shared_mutex> lock(services_mtx);
        services.erase(id);
        lock.unlock();

        srv->isExiting = true;
        srv->OnExit(); // 退出前
    }

    // 发送消息
    void Send(uint32_t toId, shared_ptr<BaseMsg> msg)
    {
        shared_ptr<Service> toSrv = GetService(toId);
        if (!toSrv)
        {
            cout << "Send fail, toSrv not exist toId:" << toId << endl;
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

    // 让工作线程等待（仅工作线程调用）
    void WorkerWait()
    {
        unique_lock<mutex> lock(sleep_mtx);
        sleep_count++;
        while (sleep_count != 0)
            sleep_cv.wait(lock);
        sleep_count--;
    }

private:
    // 获取服务
    shared_ptr<Service> GetService(uint32_t id)
    {
        shared_ptr<Service> srv = nullptr;
        shared_lock<shared_mutex> lock(services_mtx);
        unordered_map<uint32_t, shared_ptr<Service>>::const_iterator iter = services.find(id);
        if (iter != services.cend())
            srv = iter->second;
        lock.unlock();
        return srv;
    }

    // 唤醒工作线程
    void CheckAndWeakUp()
    {
        // unsafe
        if (sleep_count == 0)
            return;

        if (WORKER_NUM - sleep_count <= global_len)
        {
            cout << "weakup" << endl;
            sleep_cv.notify_all();
        }
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
};
