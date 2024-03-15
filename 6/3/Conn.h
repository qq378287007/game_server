#pragma once
using namespace std;

struct Conn
{
    enum TYPE // 消息类型
    {
        LISTEN = 1,
        CLIENT = 2,
    };

    unsigned type;
    unsigned fd;
    unsigned serviceId;
};
