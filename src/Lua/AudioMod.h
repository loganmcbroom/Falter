#pragma once

#include "../Types.h"

bool luaF_isAudioMod( lua_State * L, int i );
AudioMod luaF_checkAudioMod( lua_State *, int i );

void luaF_register_AudioMod( lua_State* L );