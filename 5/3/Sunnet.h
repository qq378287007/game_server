#pragma once
#include <thread>
#include <vector>
#include "Worker.h"
using namespace std;

class Sunnet
{
private:
    Sunnet(size_t num = thread::hardware_concurrency());

public:
    static Sunnet *inst();
    void Start();
    void Wait();

private:
    const size_t WORKER_NUM;                  // 工作线程数
    vector<Worker> workers;                   // worker对象
    vector<thread> workerThreads; // 工作线程

private:
    void StartWorker();
};
