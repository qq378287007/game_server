#include <string>
#include "ThreadPool.h"

int main()
{
    ThreadPool::instance()->Start();

    unsigned ping1 = ThreadPool::instance()->AddService("ping");
    unsigned ping2 = ThreadPool::instance()->AddService("ping");
    unsigned pong = ThreadPool::instance()->AddService("ping");

    ThreadPool::instance()->Send(make_shared<BaseMsg>(ping1, pong, "hi"));
    ThreadPool::instance()->Send(make_shared<BaseMsg>(ping2, pong, "hello"));

    this_thread::sleep_for(chrono::seconds(3));
    ThreadPool::instance()->RemoveService(ping1);
    ThreadPool::instance()->RemoveService(ping2);
    ThreadPool::instance()->RemoveService(pong);

    ThreadPool::instance()->Wait();
    return 0;
}

// g++ *.cpp -std=c++17 & a.exe
