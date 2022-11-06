#include "Function.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "Usertypes.h"
#include "Utility.h"

using namespace flan;

// Input/Output type handlers
template<typename T> struct FuncTyper { typedef void In; typedef void Out; static const int numInputs = 0; static const int numOutputs = 0; };
template<> struct FuncTyper<flan::Func1x1> { typedef float In; typedef float Out; static const int numInputs = 1; static const int numOutputs = 1; };
template<> struct FuncTyper<flan::Func2x1> { typedef vec2 In;  typedef float Out; static const int numInputs = 2; static const int numOutputs = 1; };
template<> struct FuncTyper<flan::Func2x2> { typedef vec2 In;  typedef vec2 Out;  static const int numInputs = 2; static const int numOutputs = 2; };

// These handle the fact that lua functions being cast to flan Funcs take and return 2 args
template<typename T> T luaF_F_check( lua_State * L, int i ) {}
template<> float luaF_F_check( lua_State * L, int i ) { return luaL_checknumber( L, i ); }
template<> vec2 luaF_F_check( lua_State * L, int i ) 
    { 
    if( i > 0 )
        return vec2( luaL_checknumber( L, i ), luaL_checknumber( L, i + 1 ) ); 
    else
        return vec2( luaL_checknumber( L, i - 1 ), luaL_checknumber( L, i ) ); 
    }

template<typename T> void luaF_F_push( lua_State * L, T ) {}
template<> void luaF_F_push( lua_State * L, float x ) { lua_pushnumber( L, x ); }
template<> void luaF_F_push( lua_State * L, vec2 z ) { lua_pushnumber( L, z.x() ); lua_pushnumber( L, z.y() ); }



// Interface ================================================================================================================
template<typename T>
bool luaF_isFunc( lua_State * L, int i )
    {
    return lua_isnumber( L, i ) || lua_isfunction( L, i ) || luaF_isUsertype<T>( L, i );
    }
template bool luaF_isFunc<flan::Func1x1>( lua_State * L, int i );
template bool luaF_isFunc<flan::Func2x1>( lua_State * L, int i );
template bool luaF_isFunc<flan::Func2x2>( lua_State * L, int i );

// This gets a number, lua function, or already defined flan Func type and returns a flan Func
template<typename T>
T luaF_checkFunc( lua_State * L, int i )
    {
    std::string name = luaF_getUsertypeName<T>();
    if( lua_isnumber( L, i ) )
        {
        return lua_tonumber( L, i );
        }
    else if( lua_isfunction( L, i ) ) // Lua function recieved, convert to userdata wrapped type
        {
        lua_pushvalue( L, i ); // Copy the function, ref will pop it
        const int ref = luaL_ref( L, LUA_REGISTRYINDEX );
        return [L, ref]( FuncTyper<T>::In in ) -> FuncTyper<T>::Out
            {
            lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
            luaF_F_push( L, in );
            lua_call( L, FuncTyper<T>::numInputs, FuncTyper<T>::numOutputs );
            const auto out = luaF_F_check<FuncTyper<T>::Out>( L, -1 );
            lua_pop( L, 1 );
            return out;
            };
        }
    else if( luaF_isUsertype<T>( L, i ) )
        {
        return luaF_checkUsertype<T>( L, i );
        }
    else luaL_typerror( L, i, name.c_str() );
    }

template flan::Func1x1 luaF_checkFunc( lua_State * L, int i );
template flan::Func2x1 luaF_checkFunc( lua_State * L, int i );
template flan::Func2x2 luaF_checkFunc( lua_State * L, int i );

// Metamethods ================================================================================================================

// This is called when a flan Func gets called on the Lua side.
template<typename T>
static int luaF_Func_call( lua_State * L )
    {
    const std::string name = luaF_getUsertypeName<T>();
    const int numInputs = FuncTyper<T>::numInputs;
    const int numOutputs = FuncTyper<T>::numOutputs;

    if( lua_gettop( L ) != 1 + numInputs ) 
        return luaL_error( L, ( name + " recieved the wrong number of arguments: " + std::to_string( lua_gettop( L ) - 1 ) ).c_str() );

    // Get function
    T f = luaF_check<T>( L, 1 );

    // Get args
    typename FuncTyper<T>::In in = luaF_F_check<typename FuncTyper<T>::In>( L, -1 );

    // Clear function and args
    lua_settop( L, 0 );

    // Call function and push to stack
    luaF_F_push( L, f( in ) );

    if( lua_gettop( L ) != numOutputs ) 
        return luaL_error( L, ( name + " returned the wrong number of values: " + std::to_string( lua_gettop( L ) ) ).c_str() );

    return numOutputs;
    }


// Registration ================================================================================================================
template<typename T>
static void registerFuncType( lua_State * L )
    {
    lua_register( L, luaF_getUsertypeName<T>().c_str(), luaF_Usertype_new<T> );
	luaL_newmetatable( L, luaF_getUsertypeName<T>().c_str() );
    lua_pushcfunction( L, luaF_Func_call<T> ); lua_setfield( L, -2, "__call" );
    lua_pop( L, 1 );
    }

void luaF_register_function_types( lua_State* L )
    {
    registerFuncType<flan::Func1x1>( L );
    registerFuncType<flan::Func2x1>( L );
    registerFuncType<flan::Func2x2>( L );
    }