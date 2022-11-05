#pragma once

#include <vector>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "Usertypes.h"

template<typename T> bool luaF_isArrayOfType( lua_State * L, int i );
template<typename T> std::vector<T> luaF_checkArrayOfType( lua_State * L, int i );
template<typename T> void luaF_pushArrayOfType( lua_State * L, const std::vector<T> & Ts );

template<typename T> bool luaF_is( lua_State * L, int );
template<typename T> T luaF_check( lua_State * L, int );
template<typename T> void luaF_push( lua_State * L, T );

template<typename T> concept is_vector = std::same_as<T, std::vector<typename T::value_type>>;
template<is_vector T> bool luaF_is( lua_State * L, int i ) { return luaF_isArrayOfType<typename T::value_type>( L, i ); }
template<is_vector T> T luaF_check( lua_State * L, int i ) { return luaF_checkArrayOfType<typename T::value_type>( L, i ); }
template<is_vector T> void luaF_push( lua_State * L, T v ) { return luaF_pushArrayOfType<typename T::value_type>( L, v ); }

template<typename T>
bool luaF_isArrayOfType( lua_State * L, int i )
    {
    if( ! lua_istable( L, i ) ) return false;
    const int N = lua_objlen( L, i );
    for( int j = 1; j <= N; ++j )
        {
        lua_rawgeti( L, i, j );
        if( ! luaF_is<T>( L, -1 ) ) return false;
        lua_pop( L, 1 );
        }

    return true;
    }

template<typename T>
std::vector<T> luaF_checkArrayOfType( lua_State * L, int i )
    {
    const int N = lua_objlen( L, i );
    std::vector<T> outputs;
    outputs.reserve( N );
    for( int n = 1; n <= N; ++n )
        {
        lua_rawgeti( L, i, n );
        outputs.push_back( luaF_check<T>( L, -1 ) );
        }
    return outputs;
    }

template<typename T>
void luaF_pushArrayOfType( lua_State * L, const std::vector<T> & Ts )
    {
    lua_createtable( L, Ts.size(), 0 );

    for( int i = 0; i < Ts.size(); ++i )
        {
        luaF_push( L, Ts[i] );
        lua_rawseti( L, -2, i + 1 );
        }

    if constexpr ( luaF_isUsertype<std::vector<T>>() )
        luaL_setmetatable( L, luaF_getUsertypeName<std::vector<T>>().c_str() );
    }

template<typename T>
std::vector<T> luaF_checkAsArray( lua_State * L, int i)
    {
         if( luaF_is<T>( L, i ) ) return { luaF_check<T>( L, i ) };
    else if( luaF_isArrayOfType<T>( L, i ) ) return luaF_checkArrayOfType<T>( L, i );
    else return {};
    }
