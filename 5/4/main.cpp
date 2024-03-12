#include <string>
#include "Sunnet.h"
using namespace std;

void test()
{
    shared_ptr<string> pingType(new string("ping"));
    uint32_t ping1 = Sunnet::inst()->NewService(pingType);
    uint32_t ping2 = Sunnet::inst()->NewService(pingType);
    uint32_t pong = Sunnet::inst()->NewService(pingType);
}

int main()
{
    Sunnet::inst()->Start();
    test();
    Sunnet::inst()->Wait();
    return 0;
}

// g++ *.cpp & a.exe
