#include <iostream>
using namespace std;

#include "Sunnet.h"

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
    workers.clear();
}
