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
    vector<thread> workers;

    mutex queue_mutex;
    condition_variable condition;
    bool stop{false};

private:
    // 服务列表
    unordered_map<uint32_t, shared_ptr<Service>> services;
    uint32_t maxId = 0;        // 最大ID
    shared_mutex servicesLock; // 读写锁

    // 全局队列
    queue<shared_ptr<Service>> globalQueue;
    int globalLen = 0; // 队列长度
    mutex globalLock;  // 锁

    // 休眠和唤醒
    mutex sleepMtx;
    condition_variable sleepCond;
    int sleepCount = 0; // 休眠工作线程数

public:
    ThreadPool(size_t threads = thread::hardware_concurrency())
    {
        cout << "Enter ThreadPool" << endl;

        for (size_t i = 0; i < threads; ++i)
        {
            workers.emplace_back([this]
                                 {
                    while (true)
                    {
                        shared_ptr<Service> srv = PopGlobalQueue();
                        if (!srv)
                        {
                           WorkerWait();
                        }
                        else
                        {
                            //srv->ProcessMsgs(eachNum);
                            CheckAndPutGlobal(srv);
                        }
                    } });
        }
    }
    ~ThreadPool()
    {
        {
            unique_lock<mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (thread &worker : workers)
            worker.join();

        cout << "Leave ThreadPool" << endl;
    }

    // 增删服务
    uint32_t NewService(shared_ptr<string> type)
    {
        shared_ptr<Service> srv(new Service());
        srv->type = type;
        unique_lock<shared_mutex> lock(servicesLock);
        srv->id = maxId++;
        services.emplace(srv->id, srv);
        lock.unlock();
        srv->OnInit(); // 初始化
        return srv->id;
    }

    void KillService(uint32_t id) // 仅限服务自己调用
    {
        shared_ptr<Service> srv = GetService(id);
        if (!srv)
            return;

        // 退出前
        srv->isExiting = true;
        srv->OnExit();

        unique_lock<shared_mutex> lock(servicesLock);
        services.erase(id);
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
        // 检查并放入全局队列
        // 为缩小临界区灵活控制，破坏封装性
        bool hasPush = false;

        unique_lock<mutex> lock(toSrv->inGlobalLock);
        if (!toSrv->inGlobal)
        {
            PushGlobalQueue(toSrv);
            toSrv->inGlobal = true;
            hasPush = true;
        }
        lock.unlock();

        // 唤起进程，不放在临界区里面
        if (hasPush)
            CheckAndWeakUp();
    }

    // 全局队列操作
    shared_ptr<Service> PopGlobalQueue()
    {
        shared_ptr<Service> srv = nullptr;
        {
            unique_lock<mutex> lock(globalLock);
            if (!globalQueue.empty())
            {
                srv = globalQueue.front();
                globalQueue.pop();
                globalLen--;
            }
        }
        return srv;
    }

    void PushGlobalQueue(shared_ptr<Service> srv)
    {
        unique_lock<mutex> lock(globalLock);
        globalQueue.push(srv);
        globalLen++;
    }

    // 让工作线程等待（仅工作线程调用）
    void WorkerWait()
    {
        unique_lock<mutex> lock(sleepMtx);
        while (sleepCount == 0)
            sleepCond.wait(lock);
    }

private:
    // 唤醒工作线程
    void CheckAndWeakUp()
    {
        // unsafe
        if (sleepCount == 0)
            return;

        if (WORKER_NUM - sleepCount <= globalLen)
        {
            cout << "weakup" << endl;
            sleepCond.notify_all();
        }
    }

    // 获取服务
    shared_ptr<Service> GetService(uint32_t id)
    {
        shared_ptr<Service> srv = nullptr;
        shared_lock<shared_mutex> lock(servicesLock);
        unordered_map<uint32_t, shared_ptr<Service>>::iterator iter = services.find(id);
        if (iter != services.end())
            srv = iter->second;
        lock.unlock();
        return srv;
    }

    void CheckAndPutGlobal(shared_ptr<Service> srv)
    {
        // 退出中（只能自己调退出，isExiting不会线程冲突）
        if (srv->isExiting)
            return;

        unique_lock<mutex> lock(srv->queueLock);
        if (!srv->msgQueue.empty()) // 重新放回全局队列
            PushGlobalQueue(srv);   // 此时srv->inGlobal一定是true
        else
            srv->SetInGlobal(false); // 不在队列中，重设inGlobal
    }
};
