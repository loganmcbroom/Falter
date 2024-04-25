#include "Interpolators.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "Utility.h"
#include "LTMP.h"

flan::Interpolator index2interpolator( InterpolatorIndex i )
    {
    switch( i )
        {
        case InterpolatorIndex::midpoint:        return flan::Interpolator::midpoint();        
        case InterpolatorIndex::nearest:         return flan::Interpolator::nearest();         
        case InterpolatorIndex::floor:           return flan::Interpolator::floor();           
        case InterpolatorIndex::ceil:            return flan::Interpolator::ceil();            
        case InterpolatorIndex::linear:          return flan::Interpolator::linear();          
        case InterpolatorIndex::smoothstep:      return flan::Interpolator::smoothstep();      
        case InterpolatorIndex::smootherstep:    return flan::Interpolator::smootherstep();    
        case InterpolatorIndex::sine:            return flan::Interpolator::sine();            
        case InterpolatorIndex::sqrt:            return flan::Interpolator::sqrt();            
        default: return flan::Interpolator::linear();
        }
    }

flan::Interpolator underlying2Interpolator( std::underlying_type_t<InterpolatorIndex> i )
    {
    return index2interpolator( static_cast<InterpolatorIndex>( i ) );
    }

void luaF_register_Interpolators( lua_State * L )
    {
    luaL_newmetatable( L, luaF_getUsertypeName<InterpolatorIndex>().c_str() );
        lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" ); // I need to look up why this works this way
        // lua_pushcclosure( L, luaF_LTMP<F_Interpolation_spline,                 1>, 0 ); lua_setfield( L, -2, "spline" );

        lua_pushstring( L, "midpoint" );        luaF_push( L, InterpolatorIndex::midpoint     ); lua_rawset( L, -3 );
        lua_pushstring( L, "nearest" );         luaF_push( L, InterpolatorIndex::nearest      ); lua_rawset( L, -3 );
        lua_pushstring( L, "floor" );           luaF_push( L, InterpolatorIndex::floor        ); lua_rawset( L, -3 );
        lua_pushstring( L, "ceil" );            luaF_push( L, InterpolatorIndex::ceil         ); lua_rawset( L, -3 );
        lua_pushstring( L, "linear" );          luaF_push( L, InterpolatorIndex::linear       ); lua_rawset( L, -3 );
        lua_pushstring( L, "smoothstep" );      luaF_push( L, InterpolatorIndex::smoothstep   ); lua_rawset( L, -3 );
        lua_pushstring( L, "smootherstep" );    luaF_push( L, InterpolatorIndex::smootherstep ); lua_rawset( L, -3 );
        lua_pushstring( L, "sine" );            luaF_push( L, InterpolatorIndex::sine         ); lua_rawset( L, -3 );
        lua_pushstring( L, "sqrt" );            luaF_push( L, InterpolatorIndex::sqrt         ); lua_rawset( L, -3 );
    lua_setglobal( L, luaF_getUsertypeName<InterpolatorIndex>().c_str() );
    }