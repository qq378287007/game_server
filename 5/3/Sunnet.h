#pragma once

#include <thread>
#include <vector>
using namespace std;

#include "Worker.h"

class Sunnet
{
private:
    Sunnet(unsigned num = thread::hardware_concurrency());

public:
    static Sunnet *inst();
    void Start();
    void Wait();

private:
    const unsigned WORKER_NUM;    // 工作线程数
    vector<Worker> workers;       // worker对象
    vector<thread> workerThreads; // 工作线程
};
