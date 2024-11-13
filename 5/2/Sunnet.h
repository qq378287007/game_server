#pragma once

#include <thread>
#include <vector>
using namespace std;

class Sunnet
{
    const unsigned WORKER_NUM;    // 工作线程数
    vector<thread> workerThreads; // 工作线程
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
};
