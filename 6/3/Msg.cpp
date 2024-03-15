#include <thread>
#include <chrono>
#include "ThreadPool.h"
#include "Msg.h"

BaseMsg::~BaseMsg()
{
}

BaseMsg::BaseMsg(unsigned _from, unsigned _to, const string &_buff)
    : from(_from), to(_to), buff(_buff)
{
}

void BaseMsg::run()
{
    cout << "From " << from << " To " << to << ", Buff: " << buff << endl;

    this_thread::sleep_for(chrono::milliseconds(100));
    ThreadPool::instance()->Send(make_shared<BaseMsg>(to, from, "ping"));
}

void SocketAcceptMsg::run()
{
    cout << "new conn: " << clientFd << endl;
}

void SocketRWMsg::run()
{
    if(isRead){
        char buff[512];
        int len = read(fd, &buff, 512);
        if(len>0){
            cout << "read " << string(buff, len) << endl;
            char wirteBuff[3] = {'l','p','y'};
            write(fd, &wirteBuff, 3);
        } else {
            cout << "close " << fd << endl;
            ThreadPool::instance()->CloseConn(fd);
        }
    }
}
