#include <iostream>
using namespace std;

#include "Sunnet.h"
#include "Service.h"

// 创建服务后触发
void Service::OnInit()
{
    cout << "[" << id << "] OnInit" << endl;
}
// 收到消息时触发
void Service::OnMsg(shared_ptr<BaseMsg> msg)
{
    cout << "[" << id << "] OnMsg" << endl;
    msg->run();

    this_thread::sleep_for(chrono::milliseconds(100));
    shared_ptr<BaseMsg> msgRet(new BaseMsg(id, msg->from, "ping"));
    Sunnet::inst()->Send(msgRet);
}
// 退出服务时触发
void Service::OnExit()
{
    cout << "[" << id << "] OnExit" << endl;
}

// 取出消息
shared_ptr<BaseMsg> Service::PopMsg()
{
    lock_guard lock(msg_mtx);
    if (msg_queue.empty())
        return nullptr;

    shared_ptr<BaseMsg> msg = msg_queue.front();
    msg_queue.pop();
    return msg;
}
// 插入消息
void Service::PushMsg(shared_ptr<BaseMsg> msg)
{
    msg_mtx.lock();
    msg_queue.push(msg);
    msg_mtx.unlock();
}
bool Service::ProcessMsg()
{
    shared_ptr<BaseMsg> msg = PopMsg();
    if (msg == nullptr)
        return false;

    OnMsg(msg);
    return true;
}
void Service::ProcessMsgs(int max)
{
    for (int i = 0; i < max; i++)
        if (!ProcessMsg())
            return;
}

void Service::SetInGlobal(bool isIn)
{
    inGlobalLock.lock();
    inGlobal = isIn;
    inGlobalLock.unlock();
}
