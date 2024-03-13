#include <string>
#include "ThreadPool.h"

int main()
{
    ThreadPool pool;
    pool.Start();

    pool.AddService("ping");
    pool.AddService("ping");
    pool.AddService("pong");

    for (int i = 0; i < 5; ++i)
    {
        pool.Send(make_shared<BaseMsg>(0, 2, "hi"));
        pool.Send(make_shared<BaseMsg>(1, 2, "hello"));
    }

    this_thread::sleep_for(chrono::seconds(3));
    pool.RemoveService(0);
    pool.RemoveService(1);
    pool.RemoveService(2);

    pool.Wait();
    return 0;
}

// g++ *.cpp & a.exe
