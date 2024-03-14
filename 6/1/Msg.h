#pragma once
#include <memory>
using namespace std;

struct BaseMsg // 消息基类
{
    virtual ~BaseMsg(){};

    enum TYPE // 消息类型
    {
        SERVICE = 1,
        SOCKET_ACCEPT = 2,
        SOCKET_RW = 3,
    };
    uint8_t type;        // 消息类型
    char load[999999]{}; // 用于检测内存泄漏
};

struct ServiceMsg : BaseMsg // 服务间消息
{
    uint32_t source;       // 消息发送方
    shared_ptr<char> buff; // 消息内容
    size_t size;           // 消息内容大小
};

struct SocketAcceptMsg : BaseMsg // 新连接
{
    int listenFd;
    int clientFd;
};

struct SocketRWMsg : BaseMsg // 可读可写
{
    int fd;
    bool isRead = false;
    bool isWrite = false;
};
