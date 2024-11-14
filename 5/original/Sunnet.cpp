#include <iostream>
using namespace std;

#include "Sunnet.h"

// 开启系统
void Sunnet::Start()
{
    cout << "Hello Sunnet" << endl;

    // 锁
    pthread_rwlock_init(&servicesLock, NULL);
    pthread_spin_init(&globalLock, PTHREAD_PROCESS_PRIVATE);
    pthread_cond_init(&sleepCond, NULL);
    pthread_mutex_init(&sleepMtx, NULL);

    // 开启Worker
    StartWorker();
}

// 开启worker线程
void Sunnet::StartWorker()
{
    for (int i = 0; i < WORKER_NUM; i++)
    {
        cout << "start worker thread: " << i << endl;
        // 创建线程对象
        Worker *worker = new Worker();
        worker->id = i;
        worker->eachNum = 2 << i;
        // 创建线程
        thread *wt = new thread(*worker);
        // 添加到列表
        workers.push_back(worker);
        workerThreads.push_back(wt);
    }
}

// 等待
void Sunnet::Wait()
{
    for (thread *wt : workerThreads)
        wt->join();
}

// 新建服务
uint32_t Sunnet::NewService(shared_ptr<string> type)
{
    shared_ptr<Service> srv(new Service());
    srv->type = type;

    pthread_rwlock_wrlock(&servicesLock);
    srv->id = maxId++;
    services.emplace(srv->id, srv);
    pthread_rwlock_unlock(&servicesLock);

    srv->OnInit(); // 初始化
    return srv->id;
}

// 查找服务
shared_ptr<Service> Sunnet::GetService(uint32_t id)
{
    shared_ptr<Service> srv = nullptr;
    pthread_rwlock_rdlock(&servicesLock);
    unordered_map<uint32_t, shared_ptr<Service>>::iterator iter = services.find(id);
    if (iter != services.end())
        srv = iter->second;
    pthread_rwlock_unlock(&servicesLock);
    return srv;
}

// 删除服务
// 只能service自己调自己，因为srv->OnExit、srv->isExiting不加锁
void Sunnet::KillService(uint32_t id)
{
    shared_ptr<Service> srv = GetService(id);
    if (!srv)
        return;

    // 退出前
    srv->isExiting = true;
    srv->OnExit();

    pthread_rwlock_wrlock(&servicesLock);
    services.erase(id);
    pthread_rwlock_unlock(&servicesLock);
}

// 发送消息
void Sunnet::Send(uint32_t toId, shared_ptr<BaseMsg> msg)
{
    shared_ptr<Service> toSrv = GetService(toId);
    if (!toSrv)
    {
        cout << "Send fail, toSrv not exist toId:" << toId << endl;
        return;
    }
    toSrv->PushMsg(msg);
    // 检查并放入全局队列
    // 为缩小临界区灵活控制，破坏封装性
    bool hasPush = false;
    pthread_spin_lock(&toSrv->inGlobalLock);
    if (!toSrv->inGlobal)
    {
        PushGlobalQueue(toSrv);
        toSrv->inGlobal = true;
        hasPush = true;
    }
    pthread_spin_unlock(&toSrv->inGlobalLock);

    // 唤起进程，不放在临界区里面
    if (hasPush)
        CheckAndWeakUp();
}

// 弹出全局队列
shared_ptr<Service> Sunnet::PopGlobalQueue()
{
    shared_ptr<Service> srv = nullptr;
    pthread_spin_lock(&globalLock);
    if (!globalQueue.empty())
    {
        srv = globalQueue.front();
        globalQueue.pop();
        globalLen--;
    }
    pthread_spin_unlock(&globalLock);
    return srv;
}

// 插入全局队列
void Sunnet::PushGlobalQueue(shared_ptr<Service> srv)
{
    pthread_spin_lock(&globalLock);
    globalQueue.push(srv);
    globalLen++;
    pthread_spin_unlock(&globalLock);
}

// Worker线程调用，进入休眠
void Sunnet::WorkerWait()
{
    pthread_mutex_lock(&sleepMtx);
    sleepCount++;
    pthread_cond_wait(&sleepCond, &sleepMtx);
    sleepCount--;
    pthread_mutex_unlock(&sleepMtx);
}

// 检查并唤醒线程
void Sunnet::CheckAndWeakUp()
{
    // unsafe
    if (sleepCount == 0)
        return;

    if (WORKER_NUM - sleepCount <= globalLen)
    {
        cout << "weakup" << endl;
        pthread_cond_signal(&sleepCond);
    }
}
