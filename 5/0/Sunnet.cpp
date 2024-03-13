#include <iostream>
#include <chrono>
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
    for (unsigned i = 0; i < WORKER_NUM; i++){
        workerThreads.emplace_back([i]{
            int j = 0;
            while (true)
            {
                cout << "working id: " << i << endl;
                this_thread::sleep_for(chrono::milliseconds(100));

                cout << "current j: " << j << endl;
                if(++j > 10)
                    return;
            } 
        });
    }
}
void Sunnet::Wait()
{
    for (unsigned i = 0; i < WORKER_NUM; i++)
        workerThreads[i].join();
    workerThreads.clear();
}
