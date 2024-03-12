#pragma once
#include <string>
#include <iostream>
#include <memory>
using namespace std;

struct BaseMsg // 消息基类
{
    virtual ~BaseMsg() {}
    virtual void run()
    {
        if (from < 0 || to < 0)
            return;

        cout << "From " << from << " To " << to << ", Receive Msg " << buff << endl;

        // this_thread::sleep_for(chrono::seconds(1));
        shared_ptr<BaseMsg> msgRet(new BaseMsg(to, from, string("ping")));
        // Sunnet::inst()->Send(source, msgRet);
    }

    int from;    // 消息发送方
    int to;      // 消息接收方
    string buff; // 消息内容
    BaseMsg(int _from = -1, int _to = -1, const string &_buff = "")
        : from(_from), to(_to), buff(_buff)
    {
    }
};
