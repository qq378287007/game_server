#pragma once
#include <thread>
using namespace std;
#include "Sunnet.h"
#include "Service.h"

struct Worker
{
    int id;            // 编号
    int eachNum;       // 每次处理多少条消息
    void operator()(); // 线程函数
private:
    // 辅助函数
    void CheckAndPutGlobal(shared_ptr<Service> srv);
};
