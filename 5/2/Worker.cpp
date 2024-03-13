
#include <iostream>
#include <thread>
#include <chrono>
#include "Worker.h"
using namespace std;

void Worker::operator()()
{
    int j = 0;
    while (true)
    {
        cout << "working id: " << id << endl;
        this_thread::sleep_for(chrono::milliseconds(100));

        cout << "current j: " << j << endl;
        if (++j > 10)
            return;
    }
}
