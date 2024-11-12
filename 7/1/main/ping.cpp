#include <string>
#include "ThreadPool.h"

int main()
{
    ThreadPool::instance()->Start();
    
    unsigned ping1 = ThreadPool::instance()->AddService("ping");
    unsigned ping2 = ThreadPool::instance()->AddService("ping");
    unsigned pong = ThreadPool::instance()->AddService("pong");

    this_thread::sleep_for(chrono::seconds(1));
    ThreadPool::instance()->Send(make_shared<BaseMsg>(ping1, pong, "hi"));
    ThreadPool::instance()->Send(make_shared<BaseMsg>(ping2, pong, "hello"));

    this_thread::sleep_for(chrono::seconds(3));
    ThreadPool::instance()->RemoveService(ping1);
    ThreadPool::instance()->RemoveService(ping2);
    ThreadPool::instance()->RemoveService(pong);

    this_thread::sleep_for(chrono::seconds(2));
    ThreadPool::instance()->Wait();
    return 0;
}
