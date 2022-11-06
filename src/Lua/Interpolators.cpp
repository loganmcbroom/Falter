#include "Interpolators.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

//#include <flan/Function.h>
#include <flan/Utility/Interpolator.h>

#include "Utility.h"
#include "LTMP.h"

struct F_Interpolation_interpolatePoints { flan::Func1x1 operator()( std::atomic<bool> &, const std::vector< std::pair< float, float > > & a = {}, 
    flan::Interpolator b = flan::Interpolators::linear )
    { return flan::interpolatePoints( a, b ); } };
struct F_Interpolation_interpolateIntervals { flan::Func1x1 operator()( std::atomic<bool> &, float a, const std::vector< float > b = {}, 
    flan::Interpolator c = flan::Interpolators::linear )
    { return flan::interpolateIntervals( a, b, c ); } };
struct F_Interpolation_spline { flan::Func1x1 operator()( std::atomic<bool> &, const std::vector< std::pair< float, float > > a = {} )
    { return flan::spline( a ); } };

void luaF_register_Interpolators( lua_State * L )
    {
    lua_newtable( L );
    lua_pushstring( L, "midpoint" );        luaF_push( L, flan::Interpolators::midpoint );      lua_rawset( L, -3 );
    lua_pushstring( L, "nearest" );         luaF_push( L, flan::Interpolators::nearest );       lua_rawset( L, -3 );
    lua_pushstring( L, "floor" );           luaF_push( L, flan::Interpolators::floor );         lua_rawset( L, -3 );
    lua_pushstring( L, "ceil" );            luaF_push( L, flan::Interpolators::ceil );          lua_rawset( L, -3 );
    lua_pushstring( L, "linear" );          luaF_push( L, flan::Interpolators::linear );        lua_rawset( L, -3 );
    lua_pushstring( L, "smoothstep" );      luaF_push( L, flan::Interpolators::smoothstep );    lua_rawset( L, -3 );
    lua_pushstring( L, "smootherstep" );    luaF_push( L, flan::Interpolators::smootherstep );  lua_rawset( L, -3 );
    lua_pushstring( L, "sine" );            luaF_push( L, flan::Interpolators::sine );          lua_rawset( L, -3 );
    lua_pushstring( L, "sqrt" );            luaF_push( L, flan::Interpolators::sqrt );          lua_rawset( L, -3 );
    lua_setglobal( L, "Interpolators" );
        
    // Globals defined in flan/Interpolators
    lua_pushcclosure( L, luaF_LTMP<F_Interpolation_interpolatePoints, flan::Func1x1, std::vector< std::pair< float, float > >, flan::Interpolator>, 0 ); 
    lua_setglobal( L, "interpolatePoints" );

    lua_pushcclosure( L, luaF_LTMP<F_Interpolation_interpolateIntervals, flan::Func1x1, float, std::vector<float>, flan::Interpolator>, 0 ); 
    lua_setglobal( L, "interpolateIntervals" );

    lua_pushcclosure( L, luaF_LTMP<F_Interpolation_spline, flan::Func1x1, std::vector< std::pair< float, float > >>, 0 ); 
    lua_setglobal( L, "spline" );
    }