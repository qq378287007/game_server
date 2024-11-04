#include <iostream>
using namespace std;

#include "Sunnet.h"

Sunnet::Sunnet(size_t num)
    : WORKER_NUM(num)
{
}
shared_ptr<BaseMsg> Sunnet::MakeMsg(uint32_t source, char *buff, int len)
{
    shared_ptr<ServiceMsg> msg(new ServiceMsg());
    msg->type = BaseMsg::TYPE::SERVICE;
    msg->source = source;
    msg->buff = shared_ptr<char>(buff);
    msg->size = len;
    return msg;
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
shared_ptr<Service> Sunnet::GetService(uint32_t id)
{
    shared_lock<shared_mutex> lock(servicesLock);
    unordered_map<uint32_t, shared_ptr<Service>>::const_iterator iter = services.find(id);
    if (iter != services.cend())
        return iter->second;
    return nullptr;
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
    lock_guard<mutex> lock(globalLock);
    globalQueue.push(srv);
    globalLen++;
}
// 发送消息
void Sunnet::Send(uint32_t toId, shared_ptr<BaseMsg> msg)
{
    shared_ptr<Service> toSrv = GetService(toId);
    if (!toSrv)
    {
        cout << "Send fail, toSrv not exist toId: " << toId << endl;
        return;
    }
    toSrv->PushMsg(msg);
    // 检查并放入全局队列
    // 为缩小临界区灵活控制，破坏封装性
    bool hasPush = false;
    toSrv->inGlobalLock.lock();
    if (!toSrv->inGlobal)
    {
        PushGlobalQueue(toSrv);
        toSrv->inGlobal = true;
        hasPush = true;
    }
    toSrv->inGlobalLock.unlock();
}