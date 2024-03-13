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
        cout << "start worker thread: " << i << endl;
        workers.emplace_back(i, 2U << i);
        workerThreads.emplace_back(workers[i]);
    }
}
void Sunnet::Wait()
{
    for (unsigned i = 0; i < workerThreads.size(); i++)
        workerThreads[i].join();
    workerThreads.clear();
    workers.clear();
}

unsigned Sunnet::NewService(const string &type)
{
    shared_ptr<Service> srv(new Service());
    srv->type = type;
    unique_lock<shared_mutex> lock(servicesLock);
    srv->id = maxId++;
    services.emplace(srv->id, srv);
    lock.unlock();
    srv->OnInit(); 
    return srv->id;
}
void Sunnet::KillService(unsigned id)
{
    shared_ptr<Service> srv = GetService(id);
    if (!srv)
        return;

    srv->isExiting = true;
    srv->OnExit();

    unique_lock<shared_mutex> lock(servicesLock);
    services.erase(id);
}
shared_ptr<Service> Sunnet::GetService(unsigned id)
{
    shared_ptr<Service> srv = nullptr;
    shared_lock<shared_mutex> lock(servicesLock);
    unordered_map<unsigned, shared_ptr<Service>>::const_iterator iter = services.find(id);
    if (iter != services.cend())
        srv = iter->second;
    return srv;
}
