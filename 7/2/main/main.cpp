#include <string>
#include "ThreadPool.h"

int main()
{
    ThreadPool::instance()->Start();
    
    unsigned id = ThreadPool::instance()->AddService("main");
    this_thread::sleep_for(chrono::seconds(3));
    ThreadPool::instance()->RemoveService(id);

    this_thread::sleep_for(chrono::seconds(2));
    ThreadPool::instance()->Wait();
    return 0;
}

// g++ *.cpp liblua.dll & a.exe
