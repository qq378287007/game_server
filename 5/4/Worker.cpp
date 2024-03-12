
#include <iostream>
#include <thread>
#include <chrono>
#include "Worker.h"
using namespace std;

// 线程函数
void Worker::operator()()
{
    while (true)
    {
        cout << "working id: " << id << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
}
