#include "Utility.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "Audio.h"
#include "PVOC.h"
#include "Function.h"

// luaF_check
template<typename T> 
std::vector<T> luaF_check_base( lua_State* L, int i, std::function<T ( lua_State*, int )> toFunc, std::function<bool ( lua_State*, int )> isFunc )
    {
    if( isFunc( L, i ) )
        return { (T) toFunc( L, i ) };
    else if( lua_istable( L, i ) )
        {
        std::vector<T> outputs;
        const int N = lua_objlen( L, i );
        for( int n = 1; n <= N; ++n )
            {
            lua_rawgeti( L, i, n );
            if( isFunc( L, -1 ) )
                outputs.push_back( toFunc( L, -1 ) );
            lua_pop( L, 1 );
            }
        return outputs;
        }
    else luaL_error( L, ( std::string("Unusable template specialization of luaF_check was invoked: ") + typeid( T ).name() ).c_str() );
    return {};
    }
template<typename T> 
std::vector<T> luaF_check( lua_State* L, int i ) 
    { 
    luaL_error( L, ( std::string("Unusable template specialization of luaF_check was invoked: ") + typeid( T ).name() ).c_str() ); 
    }

template<> std::vector<int      > luaF_check( lua_State* L, int i ) { return luaF_check_base<int>( L, i, lua_tointeger, lua_isnumber ); }
template<> std::vector<uint32_t > luaF_check( lua_State* L, int i ) { return luaF_check_base<uint32_t>( L, i, lua_tointeger, lua_isnumber ); }
template<> std::vector<float    > luaF_check( lua_State* L, int i ) { return luaF_check_base<float>( L, i, lua_tonumber, lua_isnumber ); }
template<> std::vector<s_Audio  > luaF_check( lua_State* L, int i ) { return luaF_checkAudioAsVec( L, i ); }
//template<> std::vector<s_PVOC   > luaF_check( lua_State* L, int i ) {}
template<> std::vector<s_Func1x1> luaF_check( lua_State* L, int i ) { return luaF_check_base<s_Func1x1>( L, i, luaF_checkFunc1x1, luaF_isFunc1x1 ); }


// luaF_push
template<typename R>
void luaF_push_base( lua_State* L, const std::vector<R>& os, std::function<void (lua_State*, R)> pushFunc )
    {
    if( os.size() == 0 ) luaL_error( L, "luaF_push_base was called with an empty output vector" );
    if( os.size() == 1 ) pushFunc( L, os[0] );
    else
        {
        const int N = os.size();
        lua_createtable( L, N, 0 );
        for( int i = 0; i < N; ++i )
            {
            pushFunc( L, os[i] );
            lua_rawseti( L, -2, i+1 );
            }
        }
    }
template<typename R>
void luaF_push( lua_State* L, const std::vector<R>& os ) { luaL_error( L, "Unusable template specialization of luaF_push was invoked" ); }
template<> void luaF_push( lua_State* L, const std::vector<int>& os ) { luaF_push_base<int>( L, os, lua_pushinteger); }
template<> void luaF_push( lua_State* L, const std::vector<uint32_t>& os ) { luaF_push_base<uint32_t>( L, os, lua_pushinteger); }
template<> void luaF_push( lua_State* L, const std::vector<float>& os ) { luaF_push_base<float>( L, os, lua_pushnumber ); }
template<> void luaF_push( lua_State* L, const std::vector<s_Audio>& os )
    {
    if( os.size() == 1 ) luaF_pushAudio( L, os[0] );
    else luaF_pushAudioVec( L, os );
    }

bool luaF_isUserType( lua_State * L, int i, const char * name )
    {
    if( ! lua_getmetatable( L, i ) ) return false; // | metatable from udata
    lua_pushstring( L, name ); // | metatable from udata | e.g. "Audio"
    lua_gettable( L, LUA_REGISTRYINDEX ); // | metatable from object | metatable from registry
    if( ! lua_equal( L, -2, -1 ) ) return false;
    lua_pop( L, 2 );
    return true;
    }