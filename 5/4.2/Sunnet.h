#pragma once

#include <thread>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
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
};
