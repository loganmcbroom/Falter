#pragma once

#include <vector>
#include <functional>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "./Types.h"

template<typename T> constexpr bool luaF_isUsertype() { return false; };
template<> constexpr bool luaF_isUsertype<flan::Audio>()      { return true; };
template<> constexpr bool luaF_isUsertype<flan::PVOC>()       { return true; };
template<> constexpr bool luaF_isUsertype<flan::Func1x1>()    { return true; };
template<> constexpr bool luaF_isUsertype<flan::Func2x1>()    { return true; };
template<> constexpr bool luaF_isUsertype<flan::Func2x2>()    { return true; };
template<> constexpr bool luaF_isUsertype<flan::Wavetable>()  { return true; };

template<> constexpr bool luaF_isUsertype<AudioVec>()         { return true; };
template<> constexpr bool luaF_isUsertype<PVOCVec>()          { return true; };
template<> constexpr bool luaF_isUsertype<Func1x1Vec>()       { return true; };
template<> constexpr bool luaF_isUsertype<Func2x1Vec>()       { return true; };
template<> constexpr bool luaF_isUsertype<Func2x2Vec>()       { return true; };
template<> constexpr bool luaF_isUsertype<WavetableVec>()     { return true; };

template<> constexpr bool luaF_isUsertype<AudioMod>()         { return true; };

template<typename T> std::string luaF_getUsertypeName();

void luaF_register_Usertypes( lua_State * );

template<typename T>
bool luaF_isUsertype( lua_State * L, int i )
    { 
    if( ! lua_getmetatable( L, i ) ) return false; // | metatable from udata
    lua_pushstring( L, luaF_getUsertypeName<T>().c_str() ); // | metatable from udata | e.g. "Audio"
    lua_gettable( L, LUA_REGISTRYINDEX ); // | metatable from object | metatable from registry
    if( ! lua_equal( L, -2, -1 ) ) return false;
    lua_pop( L, 2 );
    return true;
    }

template<typename T> T luaF_checkUsertype( lua_State * L, int i )
    {
    void * inputP = luaL_checkudata( L, i, luaF_getUsertypeName<T>().c_str() );
    if( ! inputP ) 
        luaL_typerror( L, i, luaF_getUsertypeName<T>().c_str() );
    T * sap = static_cast<T*>( inputP );
    return *sap;
    }

template<typename T>
void luaF_pushUsertype( lua_State * L, const T & instance )
    {
    // Create output space
    void * outputP = lua_newuserdata( L, sizeof( T ) );
    if( ! outputP ) luaL_error( L, ( luaF_getUsertypeName<T>() + " userdata couldn't be constructed." ).c_str() );

    // Construct Userdata into created space, managing a new object which is move constructed from the method output
    new(outputP) T( instance );

    // Make the userdata the type given by name
    luaL_getmetatable( L, luaF_getUsertypeName<T>().c_str() );
    if( lua_isnil( L, -1 ) )
        luaL_error( L, "luaF_pushUsertype tried to get a metatable that doesn't exist" );
    lua_setmetatable( L, -2 );
    }



// Metamethods ============================================================================================================================

template<typename T>
static int luaF_Usertype_new( lua_State* L )
    {
    // Allocate Lua space for the Userdata
    void* userData = lua_newuserdata( L, sizeof( T ) );
    if( ! userData ) return 0;

    // Construct Userdata into the space
    new(userData) T;

    luaL_setmetatable( L, luaF_getUsertypeName<T>().c_str() );
    return 1;
    }

template<typename T>
static int luaF_Usertype_vec_new( lua_State* L )
    {
    lua_createtable( L, 0, 0 );
    luaL_setmetatable( L, luaF_getUsertypeName<std::vector<T>>().c_str() );
    return 1;
    }