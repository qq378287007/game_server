#include <string>
#include "ThreadPool.h"

int main()
{
    ThreadPool::instance()->Start();

    unsigned ping1 = ThreadPool::instance()->AddService("ping");
    unsigned ping2 = ThreadPool::instance()->AddService("ping");
    unsigned pong = ThreadPool::instance()->AddService("ping");

    //ThreadPool::instance()->Send(make_shared<BaseMsg>(ping1, pong, "hi"));
    //ThreadPool::instance()->Send(make_shared<BaseMsg>(ping2, pong, "hello"));

    int fd = ThreadPool::instance()->Listen(8001, 1);

    this_thread::sleep_for(chrono::seconds(30));
    ThreadPool::instance()->RemoveService(ping1);
    ThreadPool::instance()->RemoveService(ping2);
    ThreadPool::instance()->RemoveService(pong);

    ThreadPool::instance()->CloseConn(fd);

    this_thread::sleep_for(chrono::seconds(2));
    ThreadPool::instance()->Wait();
    return 0;
}

// g++ *.cpp & ./a.out

// telnet 127.0.0.1 8001
