#pragma once

#include <vector>
#include <unordered_map>
using namespace std;

#include "Worker.h"
#include "Service.h"

class Sunnet
{
private:
    Sunnet() {}

public:
    static Sunnet *inst()
    {
        static Sunnet instance;
        return &instance;
    }

    // 仅测试用，buff须由new产生
    static shared_ptr<BaseMsg> MakeMsg(uint32_t source, char *buff, int len)
    {
        shared_ptr<ServiceMsg> msg(new ServiceMsg());
        msg->type = BaseMsg::TYPE::SERVICE;
        msg->source = source;
        msg->buff = shared_ptr<char>(buff);
        msg->size = len;
        return msg;
    }

    void Start();
    void Wait();

    // 增删服务
    uint32_t NewService(shared_ptr<string> type);
    void KillService(uint32_t id); // 仅限服务自己调用

    // 发送消息
    void Send(uint32_t toId, shared_ptr<BaseMsg> msg);
    // 全局队列操作
    shared_ptr<Service> PopGlobalQueue();
    void PushGlobalQueue(shared_ptr<Service> srv);

    // 让工作线程等待（仅工作线程调用）
    void WorkerWait();

private:
    // 工作线程
    int WORKER_NUM = 3;             // 工作线程数（配置）
    vector<Worker *> workers;       // worker对象
    vector<thread *> workerThreads; // 线程

    // 服务列表
    unordered_map<uint32_t, shared_ptr<Service>> services;
    uint32_t maxId = 0;            // 最大ID
    pthread_rwlock_t servicesLock; // 读写锁

    // 全局队列
    queue<shared_ptr<Service>> globalQueue;
    int globalLen = 0;             // 队列长度
    pthread_spinlock_t globalLock; // 锁

    // 休眠和唤醒
    pthread_mutex_t sleepMtx;
    pthread_cond_t sleepCond;
    int sleepCount = 0; // 休眠工作线程数

private:
    // 开启工作线程
    void StartWorker();

    // 唤醒工作线程
    void CheckAndWeakUp();

    // 获取服务
    shared_ptr<Service> GetService(uint32_t id);
};
