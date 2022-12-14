#include "Utility.h"

#include <flan/Audio.h>
#include <flan/PVOC.h>
#include <flan/Function.h>
#include <flan/Wavetable.h>

#include "Function.h"
#include "ExtraCheckFuncs.h"

// Is =============================================================================================================================

template<> bool luaF_is<int>( lua_State * L, int i ) { return lua_isnumber( L, i ); }
template<> bool luaF_is<uint32_t>( lua_State * L, int i ) { return lua_isnumber( L, i ); }
template<> bool luaF_is<float>( lua_State * L, int i ) { return lua_isnumber( L, i ); }
template<> bool luaF_is<bool>( lua_State * L, int i ) { return lua_isboolean( L, i ); }
template<> bool luaF_is<flan::Interval>( lua_State * L, int i ) 
    { 
    if( i < 0 ) i = lua_gettop( L ) + 1 + i;
    if( ! lua_istable( L, i ) || lua_objlen( L, i ) != 2 ) return false;
    lua_rawgeti( L, i, 1 );
    lua_rawgeti( L, i, 2 );
    const bool result = lua_isnumber( L, -2 ) && lua_isnumber( L, -1 );
    lua_pop( L, 2 );
    return result;
    }
template<> bool luaF_is<flan::Rect>( lua_State * L, int i ) 
    {
    if( i < 0 ) i = lua_gettop( L ) + 1 + i;
    if( ! lua_istable( L, i ) || lua_objlen( L, i ) != 4 ) return false;
    lua_rawgeti( L, i, 1 );
    lua_rawgeti( L, i, 2 );
    lua_rawgeti( L, i, 3 );
    lua_rawgeti( L, i, 4 );
    const bool result = lua_isnumber( L, -4 ) && lua_isnumber( L, -3 ) && lua_isnumber( L, -2 ) && lua_isnumber( L, -1 );
    lua_pop( L, 4 );
    return result;
    }
template<> bool luaF_is<std::pair<float,float>>( lua_State * L, int i ) { return luaF_is<flan::Interval>( L, i ); }
template<> bool luaF_is<flan::vec2>( lua_State * L, int i ) { return luaF_is<flan::Interval>( L, i ); }

template<> bool luaF_is<flan::Audio>( lua_State * L, int i ) { return luaF_isUsertype<flan::Audio>( L, i ); }
template<> bool luaF_is<flan::PVOC>( lua_State * L, int i ) { return luaF_isUsertype<flan::PVOC>( L, i ); }
template<> bool luaF_is<flan::Func1x1>( lua_State * L, int i ) { return luaF_isFunc<flan::Func1x1>( L, i ); }
template<> bool luaF_is<flan::Func2x1>( lua_State * L, int i ) { return luaF_isFunc<flan::Func2x1>( L, i ) || luaF_isFunc<flan::Func1x1>( L, i ); }
template<> bool luaF_is<flan::Func2x2>( lua_State * L, int i ) { return luaF_isFunc<flan::Func2x2>( L, i ); }
template<> bool luaF_is<flan::Wavetable>( lua_State * L, int i ) { return luaF_isUsertype<flan::Wavetable>( L, i ); }

template<> bool luaF_is<AudioMod>( lua_State * L, int i ) { return lua_isfunction( L, i ); }
template<> bool luaF_is<flan::PVOC::PrismFunc>( lua_State * L, int i ) { return lua_isfunction( L, i ); }

template<> bool luaF_is<std::string>( lua_State * L, int i ) { return lua_isstring( L, i ); }



// Check ==========================================================================================================================

template<> int luaF_check( lua_State * L, int i ) { return static_cast<int>( luaL_checkinteger( L, i ) ); }
template<> uint32_t luaF_check( lua_State * L, int i ) { return static_cast<uint32_t>(  luaL_checkinteger( L, i ) ); }
template<> float luaF_check( lua_State * L, int i ) { return static_cast<float>(  luaL_checknumber( L, i ) ); }
template<> bool luaF_check( lua_State * L, int i ) { return static_cast<bool>(  lua_toboolean( L, i ) ); }

template<> flan::Interval luaF_check( lua_State * L, int i ) 
    { 
    if( lua_objlen( L, i ) != 2 ) luaL_error( L, "Expected array of size 2" );
    if( i < 0 ) i = lua_gettop( L ) + 1 + i;
    lua_rawgeti( L, i, 1 );
    lua_rawgeti( L, i, 2 );
    flan::Interval interval( static_cast<float>( luaL_checknumber( L, -2 ) ), static_cast<float>( luaL_checknumber( L, -1 ) ) );
    lua_pop( L, 2 );
    return interval;
    }
template<> flan::Rect luaF_check( lua_State * L, int i ) 
    { 
    if( lua_objlen( L, i ) != 4 ) luaL_error( L, "Expected array of size 4" );
    if( i < 0 ) i = lua_gettop( L ) + 1 + i;
    lua_rawgeti( L, i, 1 );
    lua_rawgeti( L, i, 2 );
    lua_rawgeti( L, i, 3 );
    lua_rawgeti( L, i, 4 );
    flan::Rect rect( 
        static_cast<float>( luaL_checknumber( L, -4 ) ), 
        static_cast<float>( luaL_checknumber( L, -3 ) ), 
        static_cast<float>( luaL_checknumber( L, -2 ) ), 
        static_cast<float>( luaL_checknumber( L, -1  )) 
        );
    lua_pop( L, 4 );
    return rect;
    }
template<> std::pair<float,float> luaF_check( lua_State * L, int i ) 
    { 
    auto I = luaF_check<flan::Interval>( L, i );
    return { I.x1, I.x2 };
    }
template<> flan::vec2 luaF_check( lua_State * L, int i ) 
    {
    auto I = luaF_check<flan::Interval>( L, i );
    return { I.x1, I.x2 };
    }
template<> flan::MF luaF_check( lua_State * L, int i ) 
    {
    auto I = luaF_check<flan::Interval>( L, i );
    return { I.x1, I.x2 };
    }

template<> flan::Audio luaF_check( lua_State * L, int i ) { return luaF_checkUsertype<flan::Audio>( L, i ); }
template<> flan::PVOC luaF_check( lua_State * L, int i ) { return luaF_checkUsertype<flan::PVOC>( L, i ); }
template<> flan::Func1x1 luaF_check( lua_State * L, int i ) { return luaF_checkFunc<flan::Func1x1>( L, i ); }
template<> flan::Func2x1 luaF_check( lua_State * L, int i ) { return luaF_checkFunc<flan::Func2x1>( L, i ); }
template<> flan::Func2x2 luaF_check( lua_State * L, int i ) { return luaF_checkFunc<flan::Func2x2>( L, i ); }
template<> flan::Wavetable luaF_check( lua_State * L, int i ) { return luaF_checkUsertype<flan::Wavetable>( L, i ); }

template<> AudioMod luaF_check( lua_State * L, int i ) { return luaF_checkAudioMod( L, i ); }
template<> PrismFunc luaF_check( lua_State * L, int i ) { return luaF_checkPrismFunc( L, i ); }


template<> std::string luaF_check( lua_State * L, int i ) { return std::string( luaL_checkstring( L, i ) ); }



// Push ===========================================================================================================================

template<> void luaF_push( lua_State * L, int i ) { lua_pushinteger( L, i ); }
template<> void luaF_push( lua_State * L, uint32_t i ) { lua_pushinteger( L, i ); }
template<> void luaF_push( lua_State * L, float i ) { lua_pushnumber( L, i ); }
template<> void luaF_push( lua_State * L, bool i ) { lua_pushboolean( L, i ); }
template<> void luaF_push( lua_State * L, flan::Interval i ) 
    { 
    lua_newtable( L );
    lua_pushnumber( L, i.x1 ); lua_rawseti( L, -2, 1 );
    lua_pushnumber( L, i.x2 ); lua_rawseti( L, -2, 2 );
    }
template<> void luaF_push( lua_State * L, flan::Rect i ) 
    { 
    lua_newtable( L );
    lua_pushnumber( L, i.x1() ); lua_rawseti( L, -2, 1 );
    lua_pushnumber( L, i.x2() ); lua_rawseti( L, -2, 2 );
    lua_pushnumber( L, i.y1() ); lua_rawseti( L, -2, 3 );
    lua_pushnumber( L, i.y2() ); lua_rawseti( L, -2, 4 );
    }
template<> void luaF_push( lua_State * L, std::pair< float, float > z ) { luaF_push( L, flan::Interval( z.first, z.second ) ); }
template<> void luaF_push( lua_State * L, flan::vec2 z ) { luaF_push( L, flan::Interval( z.x(), z.y() ) ); }

template<> void luaF_push( lua_State * L, flan::Audio u )   { luaF_pushUsertype<flan::Audio>( L, u );   }
template<> void luaF_push( lua_State * L, flan::PVOC u )    { luaF_pushUsertype<flan::PVOC>( L, u );    }
template<> void luaF_push( lua_State * L, flan::Func1x1 u ) { luaF_pushUsertype<flan::Func1x1>( L, u );  }
template<> void luaF_push( lua_State * L, flan::Func2x1 u ) { luaF_pushUsertype<flan::Func2x1>( L, u );  }
template<> void luaF_push( lua_State * L, flan::Func2x2 u ) { luaF_pushUsertype<flan::Func2x2>( L, u );  }
template<> void luaF_push( lua_State * L, flan::Wavetable u ) { luaF_pushUsertype<flan::Wavetable>( L, u );  }

template<> void luaF_push( lua_State * L, AudioMod u )      { luaF_pushUsertype<AudioMod>( L, u );      }

template<> void luaF_push( lua_State * L, const std::string & u ) { lua_pushstring( L, u.c_str() ); }
