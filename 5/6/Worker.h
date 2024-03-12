#pragma once
#include <memory>
#include "Service.h"
using namespace std;

class Worker
{
public:
    size_t id;         // 编号
    size_t eachNum;    // 每次处理多少条消息
    void operator()(); // 线程函数
    Worker(size_t a, size_t b) {}

private:
    // 辅助函数
    void CheckAndPutGlobal(shared_ptr<Service> srv);

private:
    mutex stop_mtx;
    bool stop{false};

public:
    void SetStop();
};
