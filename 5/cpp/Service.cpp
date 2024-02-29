#include <iostream>
#include <unistd.h>
#include "Sunnet.h"
#include "Service.h"

// 插入消息
void Service::PushMsg(shared_ptr<BaseMsg> msg)
{
    unique_lock<mutex> lock(queueLock);
    msgQueue.push(msg);
}

// 取出消息
shared_ptr<BaseMsg> Service::PopMsg()
{
    unique_lock<mutex> lock(queueLock);
    if (msgQueue.empty())
        return nullptr;

    shared_ptr<BaseMsg> msg = msgQueue.front();
    msgQueue.pop();
    return msg;
}

// 处理一条消息，返回值代表是否处理
bool Service::ProcessMsg()
{
    shared_ptr<BaseMsg> msg = PopMsg();
    if (msg == nullptr)
        return false;

    OnMsg(msg);
    return true;
}

// 处理N条消息，返回值代表是否处理
void Service::ProcessMsgs(int max)
{
    for (int i = 0; i < max; i++)
        if (!ProcessMsg())
            break;
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

        sleep(1);
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

void Service::SetInGlobal(bool isIn)
{
    unique_lock<mutex> lock(inGlobalLock);
    inGlobal = isIn;
}
