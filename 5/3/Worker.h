#pragma once

struct Worker
{
    unsigned id;      // 编号
    unsigned eachNum; // 每次处理多少条消息
    Worker(unsigned _id, unsigned _eachNum)
        : id(_id), eachNum(_eachNum) {}
    void operator()();
};
