#include <iostream>
#include "ThreadPool.h"
#include "LuaAPI.h"
using namespace std;

// 注册Lua模块
void LuaAPI::Register(lua_State *luaState)
{
    static luaL_Reg lualibs[] = {
        {"AddService", AddService},
        {"RemoveService", RemoveService},
        {"Send", Send},
        {NULL, NULL}};

    luaL_newlib(luaState, lualibs);
    lua_setglobal(luaState, "sunnet");
}

// 开启新服务
int LuaAPI::AddService(lua_State *luaState)
{
    int num = lua_gettop(luaState); // 获取参数个数

    if (lua_isstring(luaState, 1) == 0) // 参数1：服务类型, 1:是 0:不是
    {
        lua_pushinteger(luaState, -1);
        return 1;
    }

    size_t len = 0;
    const char *type = lua_tolstring(luaState, 1, &len); // 获取字符串地址和长度

    unsigned id = ThreadPool::instance()->AddService(string(type, len)); // 处理
    lua_pushinteger(luaState, id); // 返回值
    return 1;
}

int LuaAPI::RemoveService(lua_State *luaState)
{
    int num = lua_gettop(luaState); // 获取参数个数
    if (lua_isinteger(luaState, 1) == 0)
        return 1;

    unsigned id = lua_tointeger(luaState, 1);
    ThreadPool::instance()->RemoveService(id); // 处理
    // 返回值, 无
    return 0;
}

// 发送消息
int LuaAPI::Send(lua_State *luaState)
{
    int num = lua_gettop(luaState); // 参数总数
    if (num != 3)
    {
        cout << "Send fail, num err" << endl;
        return 1;
    }

    if (lua_isinteger(luaState, 1) == 0)
    {
        cout << "Send fail, arg1 err" << endl;
        return 1;
    }
    unsigned from = lua_tointeger(luaState, 1); // 参数1:我是谁

    if (lua_isinteger(luaState, 2) == 0)
    {
        cout << "Send fail, arg2 err" << endl;
        return 1;
    }
    unsigned to = lua_tointeger(luaState, 2); // 参数2:发送给谁

    if (lua_isstring(luaState, 3) == 0)
    {
        cout << "Send fail, arg3 err" << endl;
        return 1;
    }
    size_t len = 0;
    const char *str = lua_tolstring(luaState, 3, &len);
    string buff(str, len); // 参数3:发送的内容

    ThreadPool::instance()->Send(make_shared<BaseMsg>(from, to, buff));
    // 返回值, 无
    return 0;
}
