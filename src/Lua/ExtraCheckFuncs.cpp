#include "ExtraCheckFuncs.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <flan/Audio.h>
#include <flan/PVOC.h>

#include "Utility.h"

AudioMod luaF_checkAudioMod( lua_State * L, int i )
    {
    if( lua_isfunction( L, i ) ) // Lua function recieved, convert to userdata wrapped type
        {
        lua_pushvalue( L, i ); // Copy the function, ref will pop it
        const int ref = luaL_ref( L, LUA_REGISTRYINDEX );
        return [L, ref]( const flan::Audio & audio, flan::Time t )
            {
            lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
            luaF_push<flan::Audio>( L, audio );
            lua_pushnumber( L, t );
            lua_call( L, 2, 1 );
            const flan::Audio out = luaF_check<flan::Audio>( L, -1 ); 
            lua_pop( L, 1 );
            return out;
            };
        }
    else 
        {
        luaL_error( L, "Non-function used in place of AudioMod" );
        return AudioMod();
        }
    }

PrismFunc luaF_checkPrismFunc( lua_State * L, int i )
    {
    if( lua_isfunction( L, i ) )
        {
        lua_pushvalue( L, i ); // Copy the function, ref will pop it
        const int ref = luaL_ref( L, LUA_REGISTRYINDEX );
        return [L, ref]( int note, flan::Time t, int harmonic, flan::Frequency contourFreq, const std::vector<flan::Magnitude> & harmonicMagnitudes ) -> flan::MF
            {
            lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
            luaF_push( L, note );
            luaF_push( L, t );
            luaF_push( L, harmonic + 1 ); // +1 for lua index
            luaF_push( L, contourFreq );
            luaF_push( L, harmonicMagnitudes );
            lua_call( L, 5, 1 );
            const flan::MF out = luaF_check<flan::MF>( L, -1 ); 
            lua_pop( L, 1 );
            return out;
            };
        }
    else 
        {
        luaL_error( L, "Non-function used in place of Prismfunc" );
        return PrismFunc();
        }
    }

// static int luaF_AudioMod_call( lua_State * L )
//     {
//     const std::string name = luaF_getUsertypeName<AudioMod>();

//     if( lua_gettop( L ) != 3 ) // 3 = 1 Userdata + 2 args
//         return luaL_error( L, ( name + " recieved the wrong number of arguments: " + std::to_string( lua_gettop( L ) - 1 ) ).c_str() );

//     // Get function
//     void * inputP = luaL_checkudata( L, 1, name.c_str() );
//     if( ! inputP ) luaL_typerror( L, 1, name.c_str() );
//     AudioMod f = *static_cast<AudioMod *>( inputP );

//     // Get args
//     flan::Audio audio = luaF_check<flan::Audio>( L, -2 );
//     const flan::Time t = luaL_checknumber( L, -1 );

//     // Clear function and args
//     lua_settop( L, 0 );

//     // Call function and push to stack
//     flan::Audio out = f( audio, t );
//     luaF_pushUsertype( L, out );

//     if( lua_gettop( L ) != 1 )
//         return luaL_error( L, ( name + " returned the wrong number of values: " + std::to_string( lua_gettop( L ) ) ).c_str() );

//     return 1;
//     }

// void luaF_register_AudioMod( lua_State* L )
//     {
//     // lua_register( L, luaF_getUsertypeName<AudioMod>().c_str(), luaF_Usertype_new<AudioMod> );
// 	// luaL_newmetatable( L, luaF_getUsertypeName<AudioMod>().c_str() );
//     // lua_pushcfunction( L, luaF_AudioMod_call ); lua_setfield( L, -2, "__call" );
//     // lua_pop( L, 1 );
//     }
   