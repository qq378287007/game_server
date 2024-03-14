#pragma once
#include "lua.hpp"

class LuaAPI
{
public:
    static void Register(lua_State *luaState);

    static int AddService(lua_State *luaState);
    static int RemoveService(lua_State *luaState);
    static int Send(lua_State *luaState);
};
