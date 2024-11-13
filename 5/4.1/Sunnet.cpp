#include <iostream>
using namespace std;

#include "Sunnet.h"

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
    {
        cout << "start worker thread: " << i << endl;
        workers.emplace_back(i, (size_t)2 << i);
        workerThreads.emplace_back(workers[i]);
    }
}

uint32_t Sunnet::NewService(shared_ptr<string> type)
{
    shared_ptr<Service> srv(new Service());
    srv->type = type;

    unique_lock<shared_mutex> lock(servicesLock);
    srv->id = maxId++;
    services.emplace(srv->id, srv);
    lock.unlock();

    srv->OnInit(); // 初始化
    return srv->id;
}
void Sunnet::KillService(uint32_t id)
{
    shared_ptr<Service> srv = GetService(id);
    if (!srv)
        return;

    // 退出前
    srv->isExiting = true;
    srv->OnExit();

    unique_lock<shared_mutex> lock(servicesLock);
    services.erase(id);
}
shared_ptr<Service> Sunnet::GetService(uint32_t id) const
{
    shared_lock<shared_mutex> lock(servicesLock);
    unordered_map<uint32_t, shared_ptr<Service>>::const_iterator iter = services.find(id);
    if (iter != services.cend())
        return iter->second;
    return nullptr;
}
