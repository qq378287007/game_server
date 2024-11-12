#include <thread>
#include <chrono>
#include <iostream>
using namespace std;

#include "ThreadPool.h"
#include "Msg.h"

void BaseMsg::run()
{
    cout << "From " << from << " To " << to << ", Buff: " << buff << endl;

    this_thread::sleep_for(chrono::milliseconds(100));
    ThreadPool::instance()->Send(make_shared<BaseMsg>(to, from, "ping"));
}
