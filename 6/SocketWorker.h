#pragma once
#include <sys/epoll.h>
#include <memory>
using namespace std;
#include "Conn.h"

class SocketWorker
{
private:
    int epollFd; // epoll描述符
public:
    void Init();       // 初始化
    void operator()(); // 线程函数
public:
    void AddEvent(int fd);
    void RemoveEvent(int fd);
    void ModifyEvent(int fd, bool epollOut);

private:
    void OnEvent(epoll_event ev);
    void OnAccept(shared_ptr<Conn> conn);
    void OnRW(shared_ptr<Conn> conn, bool r, bool w);
};
