#pragma once

#include "../Types.h"

template<typename T>
pSoundMod<T> luaF_checkSoundMod( lua_State *, int i );
pPrismFunc luaF_checkPrismFunc( lua_State * L, int i );
pGrainSource luaF_checkGrainSource( lua_State * L, int i );
template<typename T> pVecPredicate<T> luaF_checkVecPredicate( lua_State * L, int i );
template<typename T> pVecGroupFunc<T> luaF_checkVecGroupFunc( lua_State * L, int i );