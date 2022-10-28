#pragma once 

#include <flan/Function.h>

using s_Func1x1 = std::shared_ptr<flan::Func1x1>;

struct lua_State;

bool luaF_isFunc1x1( lua_State *, int i );
s_Func1x1 luaF_checkFunc1x1( lua_State * L, int i );
void luaF_pushFunc1x1( lua_State * L, s_Func1x1 audio );



void luaF_register_function_types( lua_State* L );