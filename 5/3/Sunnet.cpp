#include <iostream>
#include "Sunnet.h"
using namespace std;

Sunnet::Sunnet(unsigned num)
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
    for (unsigned i = 0; i < WORKER_NUM; i++)
    {
        workers.emplace_back(i, (size_t)2 << i);
        workerThreads.emplace_back(workers[i]);
    }
}

void Sunnet::Wait()
{
    for (unsigned i = 0; i < WORKER_NUM; i++)
        workerThreads[i].join();
    workerThreads.clear();
}
