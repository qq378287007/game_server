
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

#include "Worker.h"

void Worker::operator()() // 线程函数
{
    while (true)
    {
        cout << "working id: " << id << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
}
