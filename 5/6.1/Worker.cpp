
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

#include "Sunnet.h"
#include "Worker.h"

void Worker::CheckAndPutGlobal(shared_ptr<Service> srv)
{
    if (srv->isExiting)
        return;

    lock_guard lock(srv->msg_mtx);
    if (!srv->msg_queue.empty())              // 重新放回全局队列
        Sunnet::inst()->PushGlobalQueue(srv); // 此时srv->inGlobal一定是true
    else
        srv->SetInGlobal(false); // 不在队列中，重设inGlobal
}

void Worker::operator()()
{
    while (true)
    {

        {
            lock_guard<mutex> lock(*stop_mtx);
            if (stop)
                return;
        }
        // if (stop == true)
        //     return;

        shared_ptr<Service> srv = Sunnet::inst()->PopGlobalQueue();
        if (!srv)
        {
            Sunnet::inst()->WorkerWait();
        }
        else
        {
            srv->ProcessMsgs(eachNum);
            CheckAndPutGlobal(srv); // 已kill服务或空服务，不会放入队列中
        }
    }
}

void Worker::SetStop()
{
    // stop_mtx.lock();
    stop_mtx->lock();
    stop = true;
    stop_mtx->unlock();
    // stop_mtx.unlock();

    Sunnet::inst()->WeakUp();
}
