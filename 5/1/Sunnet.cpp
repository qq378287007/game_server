#include <iostream>
#include "Sunnet.h"
using namespace std;

Sunnet *Sunnet::inst()
{
    static Sunnet instance;
    return &instance;
}

void Sunnet::Start()
{
    cout << "Hello Sunnet" << endl;
}
