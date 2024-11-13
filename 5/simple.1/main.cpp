#include <string>
#include "ThreadPool.h"

int main()
{
    ThreadPool pool;
    pool.AddService(make_shared<string>("ping1"));
    pool.AddService(make_shared<string>("ping2"));
    for(int i=0; i<5; i++)
    {
        pool.Send(0, make_shared<BaseMsg>());
        pool.Send(1, make_shared<BaseMsg>());
    }

    pool.Wait();
    //this_thread::sleep_for(chrono::seconds(3));
    //pool.StopWorker();

    return 0;
}
// g++ *.cpp & a.exe
