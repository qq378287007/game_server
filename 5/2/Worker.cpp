
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

#include "Worker.h"

void Worker::operator()()
{
    for (int j = 0; j < 10; j++)
    {
        cout << "working id: " << id << endl;
        this_thread::sleep_for(chrono::milliseconds(100));
        cout << "current j: " << j << endl;
    }
}
