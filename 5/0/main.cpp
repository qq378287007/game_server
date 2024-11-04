#include <iostream>
#include <chrono>
using namespace std;

#include "Sunnet.h"

int main()
{
    Sunnet::inst()->Start();
    Sunnet::inst()->Wait();
    cout << "Center!\n";

    this_thread::sleep_for(chrono::seconds(3));

    Sunnet::inst()->Start();
    Sunnet::inst()->Wait();
    cout << "Over!\n";

    return 0;
}

// g++ *.cpp & a.exe
