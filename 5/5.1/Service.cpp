#include <iostream>
using namespace std;

#include "Service.h"
#include "Sunnet.h"

// 取出消息
shared_ptr<BaseMsg> Service::PopMsg()
{
    lock_guard<mutex> lock(msg_mtx);
    if (msg_queue.empty())
        return nullptr;

    shared_ptr<BaseMsg> msg = msg_queue.front();
    msg_queue.pop();
    return msg;
}

// 创建服务后触发
void Service::OnInit()
{
    cout << "[" << id << "] OnInit" << endl;
}
// 收到消息时触发
void Service::OnMsg(shared_ptr<BaseMsg> msg)
{
    // 测试用
    if (msg->type == BaseMsg::TYPE::SERVICE)
    {
        shared_ptr<ServiceMsg> m = dynamic_pointer_cast<ServiceMsg>(msg);
        cout << "[" << id << "] OnMsg " << m->buff << endl;

        this_thread::sleep_for(chrono::seconds(1));
        shared_ptr<BaseMsg> msgRet = Sunnet::MakeMsg(id, new char[9999999]{'p', 'i', 'n', 'g', '\0'}, 9999999);
        Sunnet::inst()->Send(m->source, msgRet);
    }
    else
    {
        cout << "[" << id << "] OnMsg" << endl;
    }
}
// 退出服务时触发
void Service::OnExit()
{
    cout << "[" << id << "] OnExit" << endl;
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
