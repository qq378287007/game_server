#pragma once
using namespace std;

struct Conn
{
    enum TYPE // 消息类型
    {
        LISTEN = 1,
        CLIENT = 2,
    };

    uint8_t type;
    int fd;
    uint32_t serviceId;
};
