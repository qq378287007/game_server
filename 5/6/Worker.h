#pragma once
#include <memory>
#include <thread>  
#include <mutex>
#include <atomic>
#include "Service.h"
using namespace std;

class Worker
{
public:
    unsigned id;       // 编号
    unsigned eachNum;  // 每次处理多少条消息
    Worker(unsigned _id, unsigned _eachNum)
        : id(_id), eachNum(_eachNum)
    {
        stop_mtx = new mutex();
    }
    void operator()(); 
private:
    void CheckAndPutGlobal(shared_ptr<Service> srv);

public:
    //mutex stop_mtx;
    mutex *stop_mtx; //mutex禁止拷贝构造，包含Worker成员的类，拷贝构造，拷贝赋值时会报错
    //重写拷贝构造函数或者使用指针，或者shared_ptr
    bool stop{false};

    //atomic<bool> stop{false};
public:
    void SetStop();
};
