#pragma once

#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

struct Worker
{
    unsigned id;      // 编号
    unsigned eachNum; // 每次处理多少条消息
    Worker(unsigned _id, unsigned _eachNum)
        : id(_id), eachNum(_eachNum)
    {
    }
    void operator()()
    {
        for (int j = 0; j < 10; j++)
        {
            cout << "working id: " << id << endl;
            this_thread::sleep_for(chrono::seconds(1));
            cout << "current j: " << j << endl;
        }
    }
};
