#pragma once
#include <thread>
#include <vector>
#include <memory>
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
    vector<thread> workerThreads; // 工作线程

private:
    void StartWorker();
};
