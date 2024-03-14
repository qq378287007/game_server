#include "lua.hpp"
#include <string>
#include <iostream>
using namespace std;

int main()
{
    lua_State *luaState = luaL_newstate();
    
    luaL_openlibs(luaState);

    string filename = "init1.lua";
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

// g++ init1.cpp liblua.dll -o init1.exe && init1.exe
