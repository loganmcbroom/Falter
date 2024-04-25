#include "GlobalFunctions.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "LTMP.h"
#include "Interpolators.h"

//using namespace flan;

struct F_Interpolation_interpolate_points 
    { 
    pFunc1x1 operator()( 
        const std::vector<flan::vec2> & a, 
        InterpolatorIndex b = InterpolatorIndex::linear )
        { 
        std::cout << "flan::interp_points";
        return std::make_shared<Func1x1>( flan::interpolate_points( a, index2interpolator( b ) ) ); 
        } 
    };

struct F_Interpolation_interpolate_intervals 
    { 
    pFunc1x1 operator()( 
        float a, 
        const std::vector<float> b = {}, 
        InterpolatorIndex c = InterpolatorIndex::linear )
        { 
        std::cout << "flan::interp_intervals";
        return std::make_shared<Func1x1>( flan::interpolate_intervals( a, b, index2interpolator( c ) ) ); 
        } 
    };

struct F_Func1x1_periodize 
	{ 
	pFunc1x1 operator()( 
		pFunc1x1 f, 
    	flan::Second period = 1.0f ) 
    	{ 
		std::cout << "flan::Function::periodize";
    	return std::make_shared<Func1x1>( f->periodize( period ) ); 
		} 
	};

void luaF_register_global_functions( lua_State * L )
	{
	lua_newtable( L );

	lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Interpolation_interpolate_points,     1>, 0 ); lua_setfield( L, -2, "interp_points" );
    lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Interpolation_interpolate_intervals,  2>, 0 ); lua_setfield( L, -2, "interp_intervals" );
    lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Func1x1_periodize,  1>, 0 ); lua_setfield( L, -2, "periodize" );

	lua_setglobal( L, "Flan" );
	}