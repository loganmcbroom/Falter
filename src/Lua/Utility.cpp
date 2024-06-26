#include "Utility.h"

#include <exception>

#include <flan/Audio/Audio.h>
#include <flan/PV/PV.h>
#include <flan/Function.h>
#include <flan/Wavetable.h>

#include "Function.h"
#include "ExtraCheckFuncs.h"
#include "Interpolators.h"

void dump_lua_stack( lua_State * L )
    {
    std::cout << "Lua stack dump:";
    const int top = lua_gettop(L);
    for( int i = 1; i <= top; i++ ) 
        {
        std::ostringstream ss;
        ss << "    " << i << "\t" << luaL_typename( L, i ) << " - ";
        switch( lua_type(L, i) ) {
            case LUA_TBOOLEAN:
                ss << lua_toboolean(L, i) ? "true" : "false";
                break;
            case LUA_TFUNCTION:
                ss << "";
                break;
            case LUA_TLIGHTUSERDATA:
                ss << "";
                break;
            case LUA_TNIL:
                ss << "";
                break;
            case LUA_TNONE:
                ss << "";
                break;
            case LUA_TNUMBER: 
                ss << lua_tonumber(L,i); 
                break;
            case LUA_TSTRING:
                ss << lua_tostring(L,i);
                break;
            case LUA_TTABLE:
                ss << lua_objlen( L, i ) << " elements";
                break;
            case LUA_TTHREAD:
                ss << "";
                break;
            case LUA_TUSERDATA:
                ss << "";
                break;
            }
        std::cout << ss.str();
        }
    }

// Is =============================================================================================================================

template<> bool luaF_is<int>( lua_State * L, int i ) { return lua_isnumber( L, i ); }
template<> bool luaF_is<uint16_t>( lua_State * L, int i ) { return lua_isnumber( L, i ); }
template<> bool luaF_is<uint32_t>( lua_State * L, int i ) { return lua_isnumber( L, i ); }
template<> bool luaF_is<uint64_t>( lua_State * L, int i ) { return lua_isnumber( L, i ); }
template<> bool luaF_is<unsigned long>( lua_State * L, int i ) { return lua_isnumber( L, i ); }
template<> bool luaF_is<float>( lua_State * L, int i ) { return lua_isnumber( L, i ); }
template<> bool luaF_is<Fool>( lua_State * L, int i ) { return lua_isboolean( L, i ); }
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
template<> bool luaF_is<flan::MF>( lua_State * L, int i ) { return luaF_is<flan::Interval>( L, i ); }
template<> bool luaF_is<flan::vec2>( lua_State * L, int i ) { return luaF_is<flan::Interval>( L, i ); }

template<> bool luaF_is<pAudio>( lua_State * L, int i ) { return luaF_isUsertype<pAudio>( L, i ); }
template<> bool luaF_is<pPV>( lua_State * L, int i ) { return luaF_isUsertype<pPV>( L, i ); }
template<> bool luaF_is<pFunc1x1>( lua_State * L, int i ) { return luaF_isFunc<pFunc1x1>( L, i ); }
template<> bool luaF_is<pFunc2x1>( lua_State * L, int i ) { return luaF_isFunc<pFunc2x1>( L, i ) || luaF_isFunc<pFunc1x1>( L, i ); }
template<> bool luaF_is<pFunc1x2>( lua_State * L, int i ) { return luaF_isFunc<pFunc1x2>( L, i ); }
template<> bool luaF_is<pFunc2x2>( lua_State * L, int i ) { return luaF_isFunc<pFunc2x2>( L, i ); }
template<> bool luaF_is<pWavetable>( lua_State * L, int i ) { return luaF_isUsertype<pWavetable>( L, i ); }
template<> bool luaF_is<InterpolatorIndex>( lua_State * L, int i ) { return luaF_isUsertype<InterpolatorIndex>( L, i ); }

template<> bool luaF_is<pSoundMod<flan::Audio>>( lua_State * L, int i ) { return lua_isfunction( L, i ); }
template<> bool luaF_is<pSoundMod<flan::PV>>( lua_State * L, int i ) { return lua_isfunction( L, i ); }
template<> bool luaF_is<pPrismFunc>( lua_State * L, int i ) { return lua_isfunction( L, i ); }
template<> bool luaF_is<pGrainSource>( lua_State * L, int i ) { return lua_isfunction( L, i ); }
template<> bool luaF_is<pVecPredicate<flan::Audio>>( lua_State * L, int i ) { return lua_isfunction( L, i ); }
template<> bool luaF_is<pVecGroupFunc<flan::Audio>>( lua_State * L, int i ) { return lua_isfunction( L, i ); }
template<> bool luaF_is<pVecPredicate<flan::PV>>( lua_State * L, int i ) { return lua_isfunction( L, i ); }
template<> bool luaF_is<pVecGroupFunc<flan::PV>>( lua_State * L, int i ) { return lua_isfunction( L, i ); }

template<> bool luaF_is<std::string>( lua_State * L, int i ) { return lua_isstring( L, i ); }



// Check ==========================================================================================================================

template<> int luaF_check( lua_State * L, int i ) { return static_cast<int>( luaL_checkinteger( L, i ) ); }
template<> uint16_t luaF_check( lua_State * L, int i ) { return static_cast<uint16_t>(  luaL_checkinteger( L, i ) ); }
template<> uint32_t luaF_check( lua_State * L, int i ) { return static_cast<uint32_t>(  luaL_checkinteger( L, i ) ); }
template<> uint64_t luaF_check( lua_State * L, int i ) { return static_cast<uint64_t>(  luaL_checkinteger( L, i ) ); }
template<> unsigned long luaF_check( lua_State * L, int i ) { return static_cast<unsigned long>(  luaL_checkinteger( L, i ) ); }
template<> float luaF_check( lua_State * L, int i ) { return static_cast<float>(  luaL_checknumber( L, i ) ); }
template<> Fool luaF_check( lua_State * L, int i ) { return static_cast<bool>(  lua_toboolean( L, i ) ); }

template<> flan::Interval luaF_check( lua_State * L, int i ) 
    {  
    if( lua_objlen( L, i ) != 2 ) throw std::runtime_error( "Expected array of size 2" );
    if( i < 0 ) i = lua_gettop( L ) + 1 + i;
    lua_rawgeti( L, i, 1 );
    lua_rawgeti( L, i, 2 );
    flan::Interval interval( static_cast<float>( luaL_checknumber( L, -2 ) ), static_cast<float>( luaL_checknumber( L, -1 ) ) );
    lua_pop( L, 2 );
    return interval;
    }
template<> flan::Rect luaF_check( lua_State * L, int i ) 
    { 
    if( lua_objlen( L, i ) != 4 ) throw std::runtime_error( "Expected array of size 4" );
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
  
template<> pAudio     luaF_check( lua_State * L, int i ) { return luaF_checkUsertype<pAudio>( L, i ); }
template<> pPV        luaF_check( lua_State * L, int i ) { return luaF_checkUsertype<pPV>( L, i ); }
template<> pFunc1x1   luaF_check( lua_State * L, int i ) { return luaF_checkFunc<pFunc1x1>( L, i ); }
template<> pFunc2x1   luaF_check( lua_State * L, int i ) { return luaF_checkFunc<pFunc2x1>( L, i ); }
template<> pFunc1x2   luaF_check( lua_State * L, int i ) { return luaF_checkFunc<pFunc1x2>( L, i ); }
template<> pFunc2x2   luaF_check( lua_State * L, int i ) { return luaF_checkFunc<pFunc2x2>( L, i ); }
template<> pWavetable luaF_check( lua_State * L, int i ) { return luaF_checkUsertype<pWavetable>( L, i ); }
template<> InterpolatorIndex luaF_check( lua_State * L, int i ) { return luaF_checkUsertype<InterpolatorIndex>( L, i ); }

template<> pSoundMod<flan::Audio>  luaF_check( lua_State * L, int i ) { return luaF_checkSoundMod<flan::Audio>( L, i ); }
template<> pSoundMod<flan::PV>  luaF_check( lua_State * L, int i ) { return luaF_checkSoundMod<flan::PV>( L, i ); }
template<> pPrismFunc luaF_check( lua_State * L, int i ) { return luaF_checkPrismFunc( L, i ); }
template<> pGrainSource luaF_check( lua_State * L, int i ) { return luaF_checkGrainSource( L, i ); }
template<> pVecPredicate<flan::Audio> luaF_check( lua_State * L, int i ) { return luaF_checkVecPredicate<flan::Audio>( L, i ); }
template<> pVecGroupFunc<flan::Audio> luaF_check( lua_State * L, int i ) { return luaF_checkVecGroupFunc<flan::Audio>( L, i ); }
template<> pVecPredicate<flan::PV> luaF_check( lua_State * L, int i ) { return luaF_checkVecPredicate<flan::PV>( L, i ); }
template<> pVecGroupFunc<flan::PV> luaF_check( lua_State * L, int i ) { return luaF_checkVecGroupFunc<flan::PV>( L, i ); }

template<> std::string luaF_check( lua_State * L, int i ) { return std::string( luaL_checkstring( L, i ) ); }



// Push ===========================================================================================================================

template<> void luaF_push( lua_State * L, int i ) { lua_pushinteger( L, i ); }
template<> void luaF_push( lua_State * L, uint32_t i ) { lua_pushinteger( L, i ); }
template<> void luaF_push( lua_State * L, float i ) { lua_pushnumber( L, i ); }
template<> void luaF_push( lua_State * L, Fool i ) { lua_pushboolean( L, i.b ); }
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

template<> void luaF_push( lua_State * L, pAudio u )            { luaF_pushUsertype<pAudio>( L, u );            }
template<> void luaF_push( lua_State * L, pPV u )               { luaF_pushUsertype<pPV>( L, u );               }
template<> void luaF_push( lua_State * L, pFunc1x1 u )          { luaF_pushUsertype<pFunc1x1>( L, u );          }
template<> void luaF_push( lua_State * L, pFunc2x1 u )          { luaF_pushUsertype<pFunc2x1>( L, u );          }
template<> void luaF_push( lua_State * L, pFunc1x2 u )          { luaF_pushUsertype<pFunc1x2>( L, u );          }
template<> void luaF_push( lua_State * L, pFunc2x2 u )          { luaF_pushUsertype<pFunc2x2>( L, u );          }
template<> void luaF_push( lua_State * L, pWavetable u )        { luaF_pushUsertype<pWavetable>( L, u );        }

template<> void luaF_push( lua_State * L, pSoundMod<flan::Audio> u )         { luaF_pushUsertype<pSoundMod<flan::Audio>>( L, u );         }
template<> void luaF_push( lua_State * L, pSoundMod<flan::PV> u )         { luaF_pushUsertype<pSoundMod<flan::PV>>( L, u );         }
template<> void luaF_push( lua_State * L, pPrismFunc u )        { luaF_pushUsertype<pPrismFunc>( L, u );        }
template<> void luaF_push( lua_State * L, pGrainSource u )      { luaF_pushUsertype<pGrainSource>( L, u );      }
template<> void luaF_push( lua_State * L, pVecPredicate<flan::Audio> u )   { luaF_pushUsertype<pVecPredicate<flan::Audio>>( L, u );      }
template<> void luaF_push( lua_State * L, pVecGroupFunc<flan::Audio> u )   { luaF_pushUsertype<pVecGroupFunc<flan::Audio>>( L, u );      }
template<> void luaF_push( lua_State * L, pVecPredicate<flan::PV> u )   { luaF_pushUsertype<pVecPredicate<flan::PV>>( L, u );      }
template<> void luaF_push( lua_State * L, pVecGroupFunc<flan::PV> u )   { luaF_pushUsertype<pVecGroupFunc<flan::PV>>( L, u );      }

template<> void luaF_push( lua_State * L, InterpolatorIndex u ) { luaF_pushUsertype<InterpolatorIndex>( L, u ); }

template<> void luaF_push( lua_State * L, const std::string & u ) { lua_pushstring( L, u.c_str() ); }
