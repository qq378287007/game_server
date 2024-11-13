
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
        cout << "current j: " << j << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
}