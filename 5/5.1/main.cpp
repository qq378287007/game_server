#include <string>
using namespace std;

#include "Sunnet.h"

void test()
{
    shared_ptr<string> pingType(new string("ping"));
    uint32_t ping1 = Sunnet::inst()->NewService(pingType);
    uint32_t ping2 = Sunnet::inst()->NewService(pingType);
    uint32_t pong = Sunnet::inst()->NewService(pingType);

    shared_ptr<BaseMsg> msg1 = Sunnet::inst()->MakeMsg(ping1, new char[3]{'h', 'i', '\0'}, 3);
    Sunnet::inst()->Send(pong, msg1);

    shared_ptr<BaseMsg> msg2 = Sunnet::inst()->MakeMsg(ping2, new char[6]{'h', 'e', 'l', 'l', 'o', '\0'}, 6);
    Sunnet::inst()->Send(pong, msg2);

    this_thread::sleep_for(chrono::seconds(3));
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
