#include <string>
#include <iostream>
#include <memory>
using namespace std;

#include "lua.hpp"

int NewService(lua_State *luaState)
{
    // 栈顶元素索引|栈上元素个数（参数个数），1
    int num = lua_gettop(luaState);

    // 是否字符串或数字（数字总能转换成字符串），1:是 0:不是
    if (lua_isstring(luaState, 1) == 0)
    {
        lua_pushinteger(luaState, -1);
        return 1;
    }

    size_t len = 0;
    const char *type = lua_tolstring(luaState, 1, &len);
    char *newstr = new char[len + 1]; // 后面加\0
    newstr[len] = '\0';
    memcpy(newstr, type, len);
    shared_ptr<string> t(new string(newstr));
    delete[] newstr;

    // uint32_t id = Sunnet::inst->NewService(t);// 处理
    static uint32_t id = 0;

    lua_pushinteger(luaState, id++); // 返回值
    return 1;
}

void Register(lua_State *luaState)
{
    static luaL_Reg lualibs[] = {
        {"NewService", NewService},
        //{ "KillService", KillService },
        //{ "Send", Send },

        //{ "Listen", Listen },
        //{ "CloseConn", CloseConn },
        //{ "Write", Write },
        {NULL, NULL}};

    luaL_newlib(luaState, lualibs);
    lua_setglobal(luaState, "sunnet");
}

int main()
{
    lua_State *luaState = luaL_newstate();

    luaL_openlibs(luaState);

    Register(luaState);

    string filename = "init2.lua";
    int isok = luaL_dofile(luaState, filename.data());
    if (isok == 1)
        cout << "error!\n";

    lua_getglobal(luaState, "OnInit");
    lua_pushinteger(luaState, 123);
    isok = lua_pcall(luaState, 1, 0, 0);
    if (isok != 0)
        cout << "error: " << lua_tostring(luaState, -1) << endl;

    lua_getglobal(luaState, "OnExit");
    isok = lua_pcall(luaState, 0, 0, 0);
    if (isok != 0)
        cout << "error: " << lua_tostring(luaState, -1) << endl;

    lua_close(luaState);
    return 0;
}

// g++ init2.cpp liblua.dll -o init2.exe && init2.exe
