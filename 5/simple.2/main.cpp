#include <string>
#include "ThreadPool.h"

int main()
{
    ThreadPool pool;
    pool.Start();

    unsigned ping0 = pool.AddService("ping");
    unsigned ping1 = pool.AddService("ping");
    unsigned pong = pool.AddService("pong");

    for (int i = 0; i < 5; ++i)
    {
        pool.Send(make_shared<BaseMsg>(ping0, pong, "hi"));
        pool.Send(make_shared<BaseMsg>(ping1, pong, "hello"));
    }

    this_thread::sleep_for(chrono::seconds(3));
    pool.RemoveService(ping0);
    pool.RemoveService(ping1);
    pool.RemoveService(pong);

    pool.Wait();
    return 0;
}

// g++ *.cpp -std=c++17 & a.exe
