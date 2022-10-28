#include "Function.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "Utility.h"

#define LUA_FLAN_FUNC1X1 "Func1x1"

using namespace flan;

// Interface ================================================================================================================

bool luaF_isFunc1x1( lua_State * L, int i )
    {
    return lua_isnumber( L, i ) || lua_isfunction( L, i ) || luaF_isUserType( L, i, LUA_FLAN_FUNC1X1 );
    }

s_Func1x1 luaF_checkFunc1x1( lua_State * L, int i )
    {
    if( lua_isnumber( L, i ) )
        {
        const float x = lua_tonumber( L, i );
        return std::make_shared<Func1x1>( [x]( float )
            { 
            return x;
            } );
        }
    else if( lua_isfunction( L, i ) ) // Lua function recieved, convert to userdata wrapped type
        {
        const int ref = luaL_ref( L, LUA_REGISTRYINDEX );
        return std::make_shared<Func1x1>( [L, ref]( float x )
            { 
            lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
            lua_pushnumber( L, x );
            lua_call( L, 1, 1 );
            const float out = luaL_checknumber( L, -1 );
            lua_pop( L, 1 );
            return out;
            } );
        }
    else if( luaF_isUserType( L, i, LUA_FLAN_FUNC1X1 ) )
        {
        void * inputP = luaL_checkudata( L, i, LUA_FLAN_FUNC1X1 );
        if( ! inputP ) luaL_typerror( L, i, LUA_FLAN_FUNC1X1 );
        s_Func1x1 * sfp = static_cast<s_Func1x1 *>( inputP );
        return *sfp;
        }
    else luaL_error( L, "Non-function recieved by luaF_checkFunc1x1" );
    }

void luaF_pushFunc1x1( lua_State * L, s_Func1x1 func )
    {
    // Create output space
    void * outputP = lua_newuserdata( L, sizeof( s_Func1x1 ) );
    if( ! outputP ) luaL_error( L, "Func1x1 userdata couldn't be constructed." );

    // Construct shared ptr into created space, managing a new Audio which is move constructed from the method output
    new(outputP) s_Func1x1( func );

    // Make the userdata a Lua Audio
    luaL_setmetatable( L, LUA_FLAN_FUNC1X1 );
    }



// Metamethods ================================================================================================================

static int luaF_func1x1_new( lua_State* L )
    {
    // Allocate Lua space for the shared_ptr
    void* userData = lua_newuserdata( L, sizeof( s_Func1x1 ) );
    if( ! userData ) return 0;

    // Construct shared_ptr into the space
    new(userData) s_Func1x1;

    luaL_setmetatable( L, LUA_FLAN_FUNC1X1 );
    return 1;
    }

static int luaF_func1x1_call( lua_State* L )
    {
    if( lua_gettop( L ) != 2 ) 
        return luaL_error( L, ( std::string("Func1x1 recieved the wrong number of arguments: ") + std::to_string( lua_gettop( L ) - 1 ) ).c_str() );

    // Get function
    void * inputP = luaL_checkudata( L, 1, LUA_FLAN_FUNC1X1 );
    if( ! inputP ) luaL_typerror( L, 1, LUA_FLAN_FUNC1X1 );
    flan::Func1x1 & f = **static_cast<s_Func1x1 *>( inputP );

    // Get number
    const int x = luaL_checknumber( L, 2 );

    // Clear function and arg
    lua_settop( L, 0 );

    // Call function and push to stack
    lua_pushnumber( L, f(x) );

    return 1;
    }


// Registration ================================================================================================================


void luaF_register_function_types( lua_State* L )
    {
    // Create LUA_FLAN_FUNC1X1 type
    lua_register( L, LUA_FLAN_FUNC1X1, luaF_func1x1_new );
	luaL_newmetatable( L, LUA_FLAN_FUNC1X1 );
    lua_pushcfunction( L, luaF_func1x1_call ); lua_setfield( L, -2, "__call" );
    lua_pop( L, 1 );
    }