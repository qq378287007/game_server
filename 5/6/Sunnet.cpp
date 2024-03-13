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
    for (unsigned i = 0; i < WORKER_NUM; i++)
    {
        workers[i].SetStop();
        workerThreads[i].join();
    }
    workers.clear();
    workerThreads.clear();
}

unsigned Sunnet::NewService(const string& type)
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

// 弹出全局队列
shared_ptr<Service> Sunnet::PopGlobalQueue()
{
    shared_ptr<Service> srv = nullptr;
    lock_guard lock(globalLock);
    if (!globalQueue.empty())
    {
        srv = globalQueue.front();
        globalQueue.pop();
        globalLen--;
    }
    return srv;
}
// 插入全局队列
void Sunnet::PushGlobalQueue(shared_ptr<Service> srv)
{
    lock_guard lock(globalLock);
    globalQueue.push(srv);
    globalLen++;
}
// 发送消息
void Sunnet::Send(shared_ptr<BaseMsg> msg)
{
    unsigned toId = msg->to;
    shared_ptr<Service> toSrv = GetService(toId);
    if (!toSrv)
    {
        cout << "Send fail, toSrv not exist toId:" << toId << endl;
        return;
    }
    toSrv->PushMsg(msg);
    
    bool hasPush = false;
    toSrv->inGlobalLock.lock();
    if (!toSrv->inGlobal)
    {
        PushGlobalQueue(toSrv);
        toSrv->inGlobal = true;
        hasPush = true;
    }
    toSrv->inGlobalLock.unlock();

    if (hasPush)
        CheckAndWeakUp();
}

void Sunnet::CheckAndWeakUp()
{
    if (sleepCount != 0 && WORKER_NUM - sleepCount <= globalLen)
        WeakUp();
}

void Sunnet::WorkerWait()
{
    sleepMtx.lock();
    sleepCount++;
    sleepMtx.unlock();

    unique_lock<mutex> lock(sleepMtx);
    sleepCond.wait(lock);
    sleepCount--;
}

void Sunnet::WeakUp()
{
    cout << "weakup" << endl;
    sleepCond.notify_all();
}
