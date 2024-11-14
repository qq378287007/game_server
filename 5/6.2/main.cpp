#include <string>
using namespace std;

#include "Sunnet.h"

void test()
{
    string pingType("ping");
    unsigned ping1 = Sunnet::inst()->NewService(pingType);
    unsigned ping2 = Sunnet::inst()->NewService(pingType);
    unsigned pong = Sunnet::inst()->NewService(pingType);

    shared_ptr<BaseMsg> msg1(new BaseMsg(pong, ping1, "hi"));
    Sunnet::inst()->Send(msg1);
    shared_ptr<BaseMsg> msg2(new BaseMsg(pong, ping2, "hello"));
    Sunnet::inst()->Send(msg2);

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
