#pragma once
#include <string>
using namespace std;

class BaseMsg // 消息基类
{
public:
    virtual ~BaseMsg();
    BaseMsg(unsigned _from = -1, unsigned _to = -1, const string &_buff = "");
    
public:
    unsigned from; // 消息发送方
    unsigned to;   // 消息接收方
    string buff;   // 消息内容
};
