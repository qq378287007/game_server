#pragma once
#include <vector>
#include <thread>
#include <unordered_map>
#include <shared_mutex>
#include <condition_variable>
#include "Worker.h"
// #include "Service.h"
using namespace std;

// class Worker;
class Sunnet
{
private:
    Sunnet(size_t num = thread::hardware_concurrency());

public:
    static Sunnet *inst();
    void Wait();

private:
    const size_t WORKER_NUM;      // 工作线程数
    vector<Worker> workers;       // worker对象
    vector<thread> workerThreads; // 工作线程
    /*
    private:
        // 服务列表
        unordered_map<uint32_t, shared_ptr<Service>> services;
        uint32_t maxId{0};         // 最大ID
        shared_mutex servicesLock; // 读写锁
    public:
        // 增删服务
        uint32_t NewService(shared_ptr<string> type);
        void KillService(uint32_t id); // 仅限服务自己调用
    private:
        // 获取服务
        shared_ptr<Service> GetService(uint32_t id);

    private:
        // 全局队列
        queue<shared_ptr<Service>> globalQueue;
        int globalLen{0}; // 队列长度
        mutex globalLock; // 锁
    public:
        // 全局队列操作
        shared_ptr<Service> PopGlobalQueue();
        void PushGlobalQueue(shared_ptr<Service> srv);
        // 发送消息
        void Send(uint32_t toId, shared_ptr<BaseMsg> msg);

    private:
        // 休眠和唤醒
        mutex sleepMtx;
        condition_variable sleepCond;
        int sleepCount{0}; // 休眠工作线程数
    public:
        // 唤醒工作线程
        void CheckAndWeakUp();
        // 让工作线程等待（仅工作线程调用）
        void WorkerWait();

    public:
        void WeakUp();
        */
};
