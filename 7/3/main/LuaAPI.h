#pragma once

#include "lua.hpp"

namespace LuaAPI
{
    void Register(lua_State *luaState);

    int AddService(lua_State *luaState);
    int RemoveService(lua_State *luaState);
    int Send(lua_State *luaState);
}
