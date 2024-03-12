#include <iostream>
#include <chrono>
#include "Sunnet.h"
using namespace std;

Sunnet::Sunnet(size_t num)
    : WORKER_NUM(num)
{
}

Sunnet *Sunnet::inst()
{
    static Sunnet instance;
    return &instance;
}

void Sunnet::Start()
{
    cout << "Hello Sunnet" << endl;
    StartWorker();
}

void Sunnet::Wait()
{
    for (size_t i = 0; i < WORKER_NUM; i++)
        workerThreads[i].join();
}

void Sunnet::StartWorker()
{
    for (size_t i = 0; i < WORKER_NUM; i++)
        workerThreads.emplace_back([i]{
            while (true)
            {
                cout << "working id: " << i << endl;
                this_thread::sleep_for(chrono::milliseconds(100));
            } 
        });
}
