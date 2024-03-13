#include <iostream>
#include "Service.h"
using namespace std;

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
    //pool.Send(make_shared<BaseMsg>(id, msg->from, "ping"));
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

void Service::SetInGlobal(bool isIn)
{
    inGlobal_mtx.lock();
    inGlobal = isIn;
    inGlobal_mtx.unlock();
}
