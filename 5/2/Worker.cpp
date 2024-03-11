
#include <unistd.h>
#include <iostream>
using namespace std;
#include "Worker.h"

// 线程函数
void Worker::operator()()
{
    while (true)
    {
        cout << "working id: " << id << endl;
        usleep(100000);
    }
}
