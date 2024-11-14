#include <string>
#include "ThreadPool.h"

int main()
{
    ThreadPool pool;
    uint32_t ping1 = pool.AddService(make_shared<string>("ping1"));
    uint32_t ping2 = pool.AddService(make_shared<string>("ping2"));
    for (int i = 0; i < 5; i++)
    {
        pool.Send(0, make_shared<BaseMsg>());
        pool.Send(1, make_shared<BaseMsg>());
    }

    // pool.Wait();

    pool.RemoveService(ping1);
    pool.RemoveService(ping2);
    this_thread::sleep_for(chrono::seconds(3));

    pool.StopWorker();

    return 0;
}

// g++ *.cpp -std=c++17 & a.exe
