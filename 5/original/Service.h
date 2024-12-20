#pragma once

#include <queue>
#include <thread>
using namespace std;

#include "Msg.h"

class Service
{
public:
    uint32_t id;             // 唯一id
    shared_ptr<string> type; // 类型
    bool isExiting = false;  // 是否正在退出

    // 消息列表
    queue<shared_ptr<BaseMsg>> msgQueue;
    pthread_spinlock_t queueLock;

    // 标记是否在全局队列  true:在队列中，或正在处理
    bool inGlobal = false;
    pthread_spinlock_t inGlobalLock;

public:
    Service();
    ~Service();

    // 回调函数（编写服务逻辑）
    void OnInit();
    void OnMsg(shared_ptr<BaseMsg> msg);
    void OnExit();

    // 插入消息
    void PushMsg(shared_ptr<BaseMsg> msg);

    // 执行消息
    bool ProcessMsg();
    void ProcessMsgs(int max);

    // 全局队列
    void SetInGlobal(bool isIn);

private:
    // 取出一条消息
    shared_ptr<BaseMsg> PopMsg();
};
