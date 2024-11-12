#include <stdio.h>
#include "lua.hpp"

int main()
{
    lua_State *luaState = luaL_newstate();

    luaL_openlibs(luaState);

    char filename[64] = "init1.lua";
    int isok = luaL_dofile(luaState, filename);
    if (isok == 1)
        printf("error!\n");

    lua_getglobal(luaState, "OnInit");
    lua_pushinteger(luaState, 123);
    isok = lua_pcall(luaState, 1, 0, 0);
    if (isok != 0)
        printf("error: %s\n", lua_tostring(luaState, -1));

    lua_getglobal(luaState, "OnExit");
    isok = lua_pcall(luaState, 0, 0, 0);
    if (isok != 0)
        printf("error: %s\n", lua_tostring(luaState, -1));

    lua_close(luaState);
    return 0;
}

// g++ init1.cpp liblua.dll -o init1.exe && init1.exe
