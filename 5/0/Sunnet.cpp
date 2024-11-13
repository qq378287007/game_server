#include <iostream>
#include <chrono>
using namespace std;

#include "Sunnet.h"

void Sunnet::Start()
{
    cout << "Hello Sunnet" << endl;
    for (unsigned i = 0; i < WORKER_NUM; i++)
    {
        workerThreads.emplace_back([i]
                                   {
            for (int j = 0; j<10;j++)
            {
                cout << "working id: " << i << endl;
                cout << "current j: " << j << endl;
                this_thread::sleep_for(chrono::milliseconds(100));
            } });
    }
}

void Sunnet::Wait()
{
    for (unsigned int i = 0; i < WORKER_NUM; i++)
        workerThreads[i].join();
    workerThreads.clear();
}
