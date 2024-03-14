#include <iostream>
#include "LuaAPI.h"
#include "Service.h"
using namespace std;

// 创建服务后触发
void Service::OnInit()
{
    cout << "[" << id << "] OnInit" << endl;

    luaState = luaL_newstate(); // 新建Lua虚拟机
    
    luaL_openlibs(luaState);

    LuaAPI::Register(luaState);//注册Sunnet系统API

    string filename = type + ".lua"; // 执行Lua文件
    int isok = luaL_dofile(luaState, filename.data());
    if (isok == 1) // 成功返回值为0，失败则为1.
        cout << "run lua fail:" << lua_tostring(luaState, -1) << endl;

    lua_getglobal(luaState, "OnInit"); // 调用Lua函数
    lua_pushinteger(luaState, id);
    isok = lua_pcall(luaState, 1, 0, 0);
    if (isok != 0) // 成功返回值为0，否则代表失败。
        cout << "call lua OnInit fail " << lua_tostring(luaState, -1) << endl;
}
// 收到消息时触发
void Service::OnMsg(shared_ptr<BaseMsg> msg)
{
    cout << "[" << id << "] OnMsg" << endl;
    msg->run();
}
// 退出服务时触发
void Service::OnExit()
{
    cout << "[" << id << "] OnExit" << endl;

    lua_getglobal(luaState, "OnExit"); // 调用Lua函数
    int isok = lua_pcall(luaState, 0, 0, 0);
    if (isok != 0) // 成功返回值为0，否则代表失败。、
        cout << "call lua OnExit fail " << lua_tostring(luaState, -1) << endl;

    lua_close(luaState); // 关闭lua虚拟机
}

// 取出消息
shared_ptr<BaseMsg> Service::PopMsg()
{
    lock_guard lock(msg_mtx);
    if (msg_queue.empty())
        return nullptr;

    shared_ptr<BaseMsg> msg = msg_queue.front();
    msg_queue.pop();
    return msg;
}
// 插入消息
void Service::PushMsg(shared_ptr<BaseMsg> msg)
{
    msg_mtx.lock();
    msg_queue.push(msg);
    msg_mtx.unlock();
}
bool Service::ProcessMsg()
{
    shared_ptr<BaseMsg> msg = PopMsg();
    if (msg == nullptr)
        return false;

    OnMsg(msg);
    return true;
}

void Service::SetInGlobal(bool isIn)
{
    inGlobal_mtx.lock();
    inGlobal = isIn;
    inGlobal_mtx.unlock();
}
