#include <string>
using namespace std;

#include "Sunnet.h"

void test()
{
    shared_ptr<string> pingType(new string("ping"));
    uint32_t ping1 = Sunnet::inst()->NewService(pingType);
    uint32_t ping2 = Sunnet::inst()->NewService(pingType);
    uint32_t pong = Sunnet::inst()->NewService(pingType);

    this_thread::sleep_for(chrono::seconds(1));
    Sunnet::inst()->KillService(ping1);
    Sunnet::inst()->KillService(ping2);
    Sunnet::inst()->KillService(pong);
}

int main()
{
    Sunnet::inst()->Start();
    test();
    Sunnet::inst()->Wait();
    return 0;
}

// g++ *.cpp -std=c++17 & a.exe
