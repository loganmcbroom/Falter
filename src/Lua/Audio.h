#pragma once

#include <memory>
#include <vector>

#include "../Types.h"

bool luaF_isAudio( lua_State *, int i );
s_Audio luaF_checkAudio( lua_State * L, int i );
void luaF_pushAudio( lua_State * L, s_Audio audio );

bool luaF_isAudioVec( lua_State * L, int i );
std::vector<s_Audio> luaF_checkAudioVec( lua_State * L, int i );
void luaF_pushAudioVec( lua_State * L, const AudioVec & audios );

std::vector<s_Audio> luaF_checkAudioAsVec( lua_State * L, int i );

void luaF_register_Audio( lua_State * );