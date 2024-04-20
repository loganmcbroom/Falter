#pragma once

#include "../Types.h"

pAudioMod luaF_checkAudioMod( lua_State *, int i );
pPrismFunc luaF_checkPrismFunc( lua_State * L, int i );
pGrainSource luaF_checkGrainSource( lua_State * L, int i );
pModIfPredicate luaF_checkModIfPredicate( lua_State * L, int i );