
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

    for (int j = 0; j < 10; j++)
    {
        cout << "working id: " << id << endl;
        cout << "current j: " << j << endl;
        shared_ptr<Service> srv = Sunnet::inst()->PopGlobalQueue();
        if (!srv)
        {
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        else
        {
            srv->ProcessMsgs(eachNum);
            CheckAndPutGlobal(srv); // 已kill服务或空服务，不会放入队列中
        }
    }
}
