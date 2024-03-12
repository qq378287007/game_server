#include "Sunnet.h"

int main()
{
    Sunnet::inst()->Start();
    Sunnet::inst()->Wait();
    return 0;
}

// g++ *.cpp & a.exe
