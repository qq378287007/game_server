
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

#include "Worker.h"
#include "Sunnet.h"

// 那些调Sunnet的通过传参数解决
// 状态是不在队列中，global=true
void Worker::CheckAndPutGlobal(shared_ptr<Service> srv)
{
    // 退出中（只能自己调退出，isExiting不会线程冲突）
    if (srv->isExiting)
        return;

    lock_guard<mutex> lock(srv->msg_mtx);
    if (srv->msg_queue.empty())
        srv->SetInGlobal(false);              // 不在队列中，重设inGlobal
    else                                      // 重新放回全局队列
        Sunnet::inst()->PushGlobalQueue(srv); // 此时srv->inGlobal一定是true
}

// 线程函数
void Worker::operator()()
{
    while (true)
    {
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
