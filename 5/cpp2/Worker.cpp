#include <iostream>
#include <unistd.h>
using namespace std;
#include "Sunnet.h"
#include "Worker.h"

// 那些调Sunnet的通过传参数解决
// 状态是不在队列中，global=true
void Worker::CheckAndPutGlobal(shared_ptr<Service> srv)
{
    // 退出中（只能自己调退出，isExiting不会线程冲突）
    if (srv->isExiting)
        return;

    pthread_spin_lock(&srv->queueLock);
    if (!srv->msgQueue.empty())               // 重新放回全局队列
        Sunnet::inst()->PushGlobalQueue(srv); // 此时srv->inGlobal一定是true
    else
        srv->SetInGlobal(false); // 不在队列中，重设inGlobal
    pthread_spin_unlock(&srv->queueLock);
}

// 线程函数
void Worker::operator()()
{
    while (true)
    {
        shared_ptr<Service> srv = Sunnet::inst()->PopGlobalQueue();
        if (!srv)
        {
            // usleep(100);
            Sunnet::inst()->WorkerWait();
        }
        else
        {
            srv->ProcessMsgs(eachNum);
            CheckAndPutGlobal(srv);
        }

        //cout << "working id: " << id << endl;
        //usleep(100000);
    }
}
