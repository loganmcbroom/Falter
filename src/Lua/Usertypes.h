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
#include "Interpolators.h"

template<typename T> constexpr bool luaF_isUsertype()           { return false; };
template<> constexpr bool luaF_isUsertype<pAudio>()             { return true; };
template<> constexpr bool luaF_isUsertype<pPV>()                { return true; };
template<> constexpr bool luaF_isUsertype<pFunc1x1>()           { return true; };
template<> constexpr bool luaF_isUsertype<pFunc2x1>()           { return true; };
template<> constexpr bool luaF_isUsertype<pFunc1x2>()           { return true; };
template<> constexpr bool luaF_isUsertype<pFunc2x2>()           { return true; };
template<> constexpr bool luaF_isUsertype<pWavetable>()         { return true; };
template<> constexpr bool luaF_isUsertype<InterpolatorIndex>()  { return true; };

template<> constexpr bool luaF_isUsertype<pAudioMod>()          { return true; };
template<> constexpr bool luaF_isUsertype<pPrismFunc>()         { return true; };
template<> constexpr bool luaF_isUsertype<pGrainSource>()       { return true; };
template<> constexpr bool luaF_isUsertype<pModIfPredicate>()    { return true; };

template<> constexpr bool luaF_isUsertype<AudioVec>()           { return true; };
template<> constexpr bool luaF_isUsertype<PVVec>()              { return true; };
template<> constexpr bool luaF_isUsertype<Func1x1Vec>()         { return true; };
template<> constexpr bool luaF_isUsertype<Func2x1Vec>()         { return true; };
template<> constexpr bool luaF_isUsertype<Func1x2Vec>()         { return true; };
template<> constexpr bool luaF_isUsertype<Func2x2Vec>()         { return true; };
template<> constexpr bool luaF_isUsertype<WavetableVec>()       { return true; };

template<typename T> std::string luaF_getUsertypeName();

template<typename T> std::string luaF_getTypeName()
    {
    if constexpr( luaF_isUsertype<T>() )
        return luaF_getUsertypeName<T>();
    else 
        return typeid( T ).name();
    }

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

// Test if a type is shared_ptr
template<typename T> struct is_shared_ptr : std::false_type {};
template<typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

// Same but for vectors
template<typename T> struct is_vector : std::false_type {};
template<typename T> struct is_vector<std::vector<T>> : std::true_type {};

template<typename T>
static int usertype_dtor( lua_State * L )
    {
    try 
        {
        void * userdataP = luaL_checkudata(L, -1, luaF_getUsertypeName<T>().c_str());

        if( userdataP ) 
            {
            auto resource = static_cast<T*>( userdataP );
            if constexpr ( is_shared_ptr<T>::value ) 
                resource->reset();
            else if constexpr ( is_vector<T>::value ) 
                resource->clear();
            }
        else throw std::runtime_error( "Couldn't retrieve userdata for deallocation." );

        return 0;
        }
    catch( std::exception & e ) { lua_pushstring( L, e.what() ); }
    lua_error( L );
    return 0;
    }

template<typename T>
void luaF_pushUsertype( lua_State * L, const T & instance )
    {
    // Create output space
    void * outputP = lua_newuserdata( L, sizeof( T ) );
    if( ! outputP ) throw std::runtime_error( luaF_getUsertypeName<T>() + " userdata couldn't be constructed." );

    // Make the userdata the type given by name. 
    // This must happen before the call to new or the dtor of the userdata will never be called by lua and it may leak a resource.
    luaL_getmetatable( L, luaF_getUsertypeName<T>().c_str() );
    if( lua_isnil( L, -1 ) )
        throw std::runtime_error( std::string( "luaF_pushUsertype tried to get a metatable that doesn't exist: " ) + typeid(T).name() );

    // Construct Userdata into created space, managing a new object which is move constructed from the method output.
    new(outputP) T( instance );

    lua_pushcfunction( L, usertype_dtor<T> );
    lua_setfield( L, -2, "__gc" );

    lua_setmetatable( L, -2 );
    }



// Metamethods ============================================================================================================================

// template<typename T>
// static int luaF_Usertype_new( lua_State* L )
//     {
//     // Allocate Lua space for the Userdata
//     void* userData = lua_newuserdata( L, sizeof( T ) );
//     if( ! userData ) return 0;

//     // Construct Userdata into the space
//     new(userData) T; woahhhhh buddy

//     luaL_setmetatable( L, luaF_getUsertypeName<T>().c_str() );
//     return 1;
//     }

template<typename T>
static int luaF_Usertype_vec_new( lua_State* L )
    {
    try 
        {
        if( ! luaF_isArrayOfType<T>( L, 1 ) ) throw std::runtime_error( "Array type constructor didn't recieve an array of the given type." );
        //lua_createtable( L, 0, 0 );
        luaL_setmetatable( L, luaF_getUsertypeName<std::vector<T>>().c_str() );
        return 1;
        }
    catch( std::exception & e ) { lua_pushstring( L, e.what() ); }
    lua_error( L );
    return 0;
    }