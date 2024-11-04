#pragma once

#include <memory>
using namespace std;

#include "Service.h"

struct Worker
{
    size_t id;      // 编号
    size_t eachNum; // 每次处理多少条消息
    Worker(size_t _id, size_t _eachNum)
        : id(_id), eachNum(_eachNum)
    {
    }
    void operator()(); // 线程函数
private:
    void CheckAndPutGlobal(shared_ptr<Service> srv); // 辅助函数
};
