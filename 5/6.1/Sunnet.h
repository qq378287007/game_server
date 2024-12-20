#pragma once

#include <thread>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <condition_variable>
using namespace std;

#include "Worker.h"
#include "Service.h"

class Sunnet
{
private:
    Sunnet(unsigned num = thread::hardware_concurrency())
        : WORKER_NUM(num) {}

public:
    static Sunnet *inst()
    {
        static Sunnet instance;
        return &instance;
    }

public:
    void Start();
    void Wait();

private:
    const unsigned WORKER_NUM;    // 工作线程数
    vector<Worker> workers;       // worker对象
    vector<thread> workerThreads; // 工作线程

private:
    // 服务列表
    unordered_map<unsigned, shared_ptr<Service>> services;
    unsigned maxId{0};         // 最大ID
    shared_mutex servicesLock; // 读写锁
public:
    // 增删服务
    unsigned NewService(const string &type);
    void KillService(unsigned id); // 仅限服务自己调用
private:
    // 获取服务
    shared_ptr<Service> GetService(unsigned id);

private:
    // 全局队列
    queue<shared_ptr<Service>> globalQueue;
    unsigned globalLen{0}; // 队列长度
    mutex globalLock;      // 锁
public:
    // 全局队列操作
    shared_ptr<Service> PopGlobalQueue();
    void PushGlobalQueue(shared_ptr<Service> srv);
    // 发送消息
    void Send(shared_ptr<BaseMsg> msg);

private:
    // 休眠和唤醒
    mutex sleepMtx;
    condition_variable sleepCond;
    unsigned sleepCount{0}; // 休眠工作线程数
public:
    // 唤醒工作线程
    void CheckAndWeakUp();
    // 让工作线程等待（仅工作线程调用）
    void WorkerWait();

public:
    void WeakUp();
};
