#include <iostream>
using namespace std;

#include "Sunnet.h"
#include "Worker.h"

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
        workerThreads.emplace_back(Worker{i, 2U << i});

        // Worker worker{i, 2U << i};
        // workerThreads.emplace_back(worker);
        // workerThreads.emplace_back(thread(worker));

        // thread th(worker);
        // workerThreads.emplace_back(move(th));
    }
}

void Sunnet::Wait()
{
    for (unsigned i = 0; i < WORKER_NUM; i++)
        workerThreads[i].join();
    workerThreads.clear();
}
