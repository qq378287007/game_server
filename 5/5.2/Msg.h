#pragma once
#include <string>
using namespace std;

struct BaseMsg // 消息基类
{
    virtual ~BaseMsg() {}
    virtual void run();

    BaseMsg(unsigned _from = -1, unsigned _to = -1, const string &_buff = "")
        : from(_from), to(_to), buff(_buff)
    {
    }

    unsigned from; // 消息发送方
    unsigned to;   // 消息接收方
    string buff;   // 消息内容
};
