#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <memory>
using namespace std;

#include "Msg.h"

struct Service
{
public:
    unsigned id;           // 唯一id
    string type;           // 类型
    bool isExiting{false}; // 是否正在退出

    // 消息列表
    queue<shared_ptr<BaseMsg>> msg_queue;
    mutex msg_mtx;

public:
    // 回调函数（编写服务逻辑）
    void OnInit();
    void OnMsg(shared_ptr<BaseMsg> msg);
    void OnExit();

    // 取出消息
    shared_ptr<BaseMsg> PopMsg();
    // 插入消息
    void PushMsg(shared_ptr<BaseMsg> msg);
    // 执行消息
    bool ProcessMsg();
    void ProcessMsgs(int max);
};
