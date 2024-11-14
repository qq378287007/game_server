#pragma once

#include <memory>
using namespace std;

struct BaseMsg // 消息基类
{
    virtual ~BaseMsg() {}
    virtual void run() {}

    enum TYPE // 消息类型
    {
        SERVICE = 1,
    };
    TYPE type;         // 消息类型
    char load[999999]; // 用于检测内存泄漏
};

struct ServiceMsg : BaseMsg // 服务间消息
{
    uint32_t source;       // 消息发送方
    shared_ptr<char> buff; // 消息内容
    size_t size;           // 消息内容大小
};
