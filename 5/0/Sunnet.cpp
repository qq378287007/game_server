#include <iostream>
#include <chrono>
using namespace std;

#include "Sunnet.h"

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
        workerThreads.emplace_back([i]
                                   {
            for (int j = 0;j<10;j++)
            {
                cout << "working id: " << i << endl;
                this_thread::sleep_for(chrono::milliseconds(100));
                cout << "current j: " << j << endl;
            } });
    }
}

void Sunnet::Wait()
{
    for (unsigned int i = 0; i < WORKER_NUM; i++)
        workerThreads[i].join();
    workerThreads.clear();
}
