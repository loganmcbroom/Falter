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
        case InterpolatorIndex::midpoint:        return flan::Interpolators::midpoint;        
        case InterpolatorIndex::nearest:         return flan::Interpolators::nearest;         
        case InterpolatorIndex::floor:           return flan::Interpolators::floor;           
        case InterpolatorIndex::ceil:            return flan::Interpolators::ceil;            
        case InterpolatorIndex::linear:          return flan::Interpolators::linear;          
        case InterpolatorIndex::smoothstep:      return flan::Interpolators::smoothstep;      
        case InterpolatorIndex::smootherstep:    return flan::Interpolators::smootherstep;    
        case InterpolatorIndex::sine:            return flan::Interpolators::sine;            
        case InterpolatorIndex::sqrt:            return flan::Interpolators::sqrt;            
        default: return flan::Interpolators::linear;
        }
    }

flan::Interpolator underlying2Interpolator( std::underlying_type_t<InterpolatorIndex> i )
    {
    return index2interpolator( static_cast<InterpolatorIndex>( i ) );
    }

// struct F_Interpolation_interpolate_points 
//     { 
//     pFunc1x1 operator()( 
//         const std::vector< std::pair< float, float > > & a, 
//         InterpolatorIndex b = InterpolatorIndex::linear )
//         { 
//         std::cout << "flan::interpolate_points";
//         return std::make_shared<Func1x1>( flan::interpolate_points( a, index2interpolator( b ) ) ); 
//         } 
//     };

// struct F_Interpolation_interpolate_intervals 
//     { 
//     pFunc1x1 operator()( float a, const std::vector< float > b = {}, flan::Interpolator c = flan::Interpolators::linear )
//         { 
//         std::cout << "flan::interpolate_intervals";
//         return std::make_shared<Func1x1>( flan::interpolate_intervals( a, b, c ) ); 
//         } 
//     };

// struct F_Interpolation_spline 
//     { 
//     pFunc1x1 operator()( const std::vector< std::pair< float, float > > a = {} )
//         { 
//         std::cout << "flan::spline";

//         return std::make_shared<Func1x1>( flan::spline( a ) ); 
//         } 
//     };

void luaF_register_Interpolators( lua_State * L )
    {
    luaL_newmetatable( L, luaF_getUsertypeName<InterpolatorIndex>().c_str() );
        lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" ); // I need to look up why this works this way

        //lua_pushcclosure( L, luaF_LTMP<F_Interpolation_interpolate_points,     1>, 0 ); lua_setfield( L, -2, "interpolate_points" );
        // lua_pushcclosure( L, luaF_LTMP<F_Interpolation_interpolate_intervals,  2>, 0 ); lua_setfield( L, -2, "interpolate_intervals" );
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