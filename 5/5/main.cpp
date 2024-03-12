#include <string>
#include "Sunnet.h"
using namespace std;

void test()
{
    shared_ptr<string> pingType(new string("ping"));
    uint32_t ping1 = Sunnet::inst()->NewService(pingType);
    uint32_t ping2 = Sunnet::inst()->NewService(pingType);
    uint32_t pong = Sunnet::inst()->NewService(pingType);
    
    shared_ptr<BaseMsg> msg1 = Sunnet::inst()->MakeMsg(ping1, new char[3]{'h', 'i', '\0'}, 3);
    shared_ptr<BaseMsg> msg2 = Sunnet::inst()->MakeMsg(ping2, new char[6]{'h', 'e', 'l', 'l', 'o', '\0'}, 6);

    Sunnet::inst()->Send(pong, msg1);
    Sunnet::inst()->Send(pong, msg2);
}

int main()
{
    Sunnet::inst()->Start();
    test();
    Sunnet::inst()->Wait();
    return 0;
}

// g++ *.cpp & a.exe
