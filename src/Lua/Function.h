#pragma once 

#include <flan/Function.h>

#include "../Types.h"

struct lua_State;

template<typename T> T luaF_checkFunc( lua_State * L, int i );
template<typename T> bool luaF_isFunc( lua_State * L, int i );

void luaF_register_function_types( lua_State* L );