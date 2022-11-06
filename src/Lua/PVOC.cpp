#include "PVOC.h"

#include "PVOC.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <flan/Audio.h>
#include <flan/PVOC.h>
#include <flan/Graph.h>

#include "LTMP.h"
#include "Usertypes.h"

using namespace flan;

// Default args
static flan::Func2x2 Func2x2Ident = []( vec2 z ){ return z; };
static flan::Func2x1 Func2x1_t = []( vec2 z ){ return z.x(); };
static flan::Func2x1 Func2x1_f = []( vec2 z ){ return z.y(); };
static flan::Func2x1 Func2x1_Arith_Ident = []( float t, float i ){ return i == 1; };

// Methods ======================================================================================================================

// Conversions
struct F_PVOC_convertToAudio { flan::Audio operator()( std::atomic<bool> & z, flan::PVOC a )
    { return a.convertToAudio( nullptr, z ); } };
struct F_PVOC_convertToGraph { flan::Graph operator()( std::atomic<bool> & z, flan::PVOC a, Rect b = { 0, 0, -1, -1 }, Pixel c = -1, Pixel d = -1, float e=0 )
    { return a.convertToGraph( b, c, d, e, z ); } };
struct F_PVOC_saveToBMP { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, const std::string & b = "PVOC.bmp", Rect c = { 0, 0, -1, -1 }, Pixel d = -1, Pixel e = -1 )
    { return a.saveToBMP( b, c, d, e, z ); } };

// Contours
// struct F_PVOC_prism { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, PrismFunc b, bool c = true )
//     { return a.prism( b, c, z ) ); } };

// Utility
struct F_PVOC_getFrame { flan::PVOC operator()( std::atomic<bool> &, flan::PVOC a, flan::Time b = 0 )
    { return a.getFrame( b ); } };

// Selection
struct F_PVOC_select { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Time b = 5, flan::Func2x2 c = Func2x2Ident, flan::Func1x1 d = flan::Interpolators::linear )
    { return a.select( b, c, d, z ); } }; 
// struct F_PVOC_freeze { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, const std::vector< std::array< flan::Time, 2 > > &b )
//     { return a.freeze( b, z ) ); } };

// Resampling
struct F_PVOC_modify_cpu { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x2 b = Func2x2Ident, flan::Func1x1 c = flan::Interpolators::linear )
    { return a.modify_cpu( b, c, z ); } }; 
struct F_PVOC_modifyFrequency_cpu { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x1 b = Func2x1_f, flan::Func1x1 c = flan::Interpolators::linear )
    { return a.modifyFrequency_cpu( b, c, z ); } };
struct F_PVOC_modifyTime_cpu { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x1 b = Func2x1_t, flan::Func1x1 c = flan::Interpolators::linear )
    { return a.modifyTime_cpu( b, c, z ); } };
struct F_PVOC_modify { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x2 b = Func2x2Ident, flan::Func1x1 c = flan::Interpolators::linear )
    { return a.modify( b, c, z ); } }; 
struct F_PVOC_modifyFrequency { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x1 b = Func2x1_f, flan::Func1x1 c = flan::Interpolators::linear )
    { return a.modifyFrequency( b, c, z ); } };
struct F_PVOC_modifyTime { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x1 b = Func2x1_t, flan::Func1x1 c = flan::Interpolators::linear )
    { return a.modifyTime( b, c, z ); } };
struct F_PVOC_repitch { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x1 b = 1, flan::Func1x1 c = flan::Interpolators::linear )
    { return a.repitch( b, c, z ); } };
struct F_PVOC_stretch { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x1 b = 1, flan::Func1x1 c = flan::Interpolators::linear )
    { return a.stretch( b, c, z ); } };
struct F_PVOC_stretch_spline { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func1x1 b = 0 )
    { return a.stretch_spline( b, z ); } };
struct F_PVOC_desample { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x1 b = 0, flan::Func1x1 c = flan::Interpolators::linear )
    { return a.desample( b, c, z ); } };
struct F_PVOC_timeExtrapolate { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Time b = 0, flan::Time c = -1, flan::Time d = 5, flan::Func1x1 e = flan::Interpolators::linear )
    { return a.timeExtrapolate( b, c, d, e, z ); } };

// Extras
struct F_PVOC_addOctaves { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x1 b = Func2x1_Arith_Ident )
    { return a.addOctaves( b, z ); } };
struct F_PVOC_addHarmonics { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x1 b = Func2x1_Arith_Ident )
    { return a.addHarmonics( b, z ); } };
struct F_PVOC_replaceAmplitudes { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::PVOC b = PVOC(), flan::Func2x1 c = 1 )
    { return a.replaceAmplitudes( b, c, z ); } };
struct F_PVOC_subtractAmplitudes { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::PVOC b = PVOC(), flan::Func2x1 c = 1 )
    { return a.subtractAmplitudes( b, c, z ); } };
struct F_PVOC_shape { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x2 b = Func2x2Ident )
    { return a.shape( b, z ); } };
struct F_PVOC_perturb { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func2x1 b = 0, flan::Func2x1 c = 0 )
    { return a.perturb( b, c, z ); } };
struct F_PVOC_retainNLoudestPartials { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func1x1 b = 0 )
    { return a.retainNLoudestPartials( b, z ); } };
struct F_PVOC_removeNLoudestPartials { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Func1x1 b = 0 )
    { return a.removeNLoudestPartials( b, z ); } };
struct F_PVOC_resonate { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, flan::Time b = 5, flan::Func2x1 c = 1 )
    { return a.resonate( b, c, z ); } };

// Static
struct F_PVOC_generate { PVOC operator()( std::atomic<bool> & z, flan::Time a = 5, Func1x1 b = 220, Func2x1 c = []( float, float i ){ return 1.0f / (i+1); } )
    { return PVOC::generate( a, b, c, z ); } };


// Registration ==============================================================================================================

void luaF_register_PVOC( lua_State * L )
    {
    // Create LUA_FLAN_PVOC type
    lua_register( L, luaF_getUsertypeName<flan::PVOC>().c_str(), luaF_Usertype_new<flan::PVOC> );
	luaL_newmetatable( L, luaF_getUsertypeName<flan::PVOC>().c_str() );
	//lua_pushcfunction( L, luaF_Usertype_delete<flan::PVOC> ); lua_setfield( L, -2, "__gc" );
    lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" ); // I need to look up why this works this way 

    // Create LUA_FLAN_PVOC_VEC type
    lua_register( L, luaF_getUsertypeName<PVOCVec>().c_str(), luaF_Usertype_vec_new<flan::PVOC> );
	luaL_newmetatable( L, luaF_getUsertypeName<PVOCVec>().c_str() );
    lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );

    // | PVOC Metatable | PVOCVec Metatable

    // Conversions
    luaF_register_helper<F_PVOC_convertToAudio, flan::Audio, flan::PVOC>( L, "convertToAudio" );
    luaF_register_helper<F_PVOC_convertToAudio, flan::Audio, flan::PVOC>( L, "__call" );
    // luaF_register_helper<F_PVOC_convertToGraph, flan::Graph, flan::PVOC, Rect, Pixel, Pixel, float>( L, "convertToGraph" );
    luaF_register_helper<F_PVOC_saveToBMP, flan::PVOC, flan::PVOC, std::string, Rect, Pixel, Pixel>( L, "saveToBMP" );


    // Contours
    // struct F_PVOC_prism { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, PrismFunc b, bool c = true )
    //     { return a.prism( b, c ) ); } };

    // Utility
    luaF_register_helper<F_PVOC_getFrame, flan::PVOC, flan::PVOC, flan::Time>( L, "getFrame" );

    // Selection
    luaF_register_helper<F_PVOC_select, flan::PVOC, flan::PVOC, flan::Time, flan::Func2x2, flan::Func1x1>( L, "select" );
    // struct F_PVOC_freeze { flan::PVOC operator()( std::atomic<bool> & z, flan::PVOC a, const std::vector< std::array< flan::Time, 2 > > &b )

    // Resampling
    luaF_register_helper<F_PVOC_modify_cpu, flan::PVOC, flan::PVOC, flan::Func2x2, flan::Func1x1>( L, "modify_cpu" );
    luaF_register_helper<F_PVOC_modifyTime_cpu, flan::PVOC, flan::PVOC, flan::Func2x1, flan::Func1x1>( L, "modifyTime_cpu" );
    luaF_register_helper<F_PVOC_modifyFrequency_cpu, flan::PVOC, flan::PVOC, flan::Func2x1, flan::Func1x1>( L, "modifyFrequency_cpu" );
    luaF_register_helper<F_PVOC_modify, flan::PVOC, flan::PVOC, flan::Func2x2, flan::Func1x1>( L, "modify" );
    luaF_register_helper<F_PVOC_modifyFrequency, flan::PVOC, flan::PVOC, flan::Func2x1, flan::Func1x1>( L, "modifyFrequency" );
    luaF_register_helper<F_PVOC_modifyTime, flan::PVOC, flan::PVOC, flan::Func2x1, flan::Func1x1>( L, "modifyTime" );
    luaF_register_helper<F_PVOC_repitch, flan::PVOC, flan::PVOC, flan::Func2x1, flan::Func1x1>( L, "repitch" );
    luaF_register_helper<F_PVOC_stretch, flan::PVOC, flan::PVOC, flan::Func2x1, flan::Func1x1>( L, "stretch" );
    luaF_register_helper<F_PVOC_stretch_spline, flan::PVOC, flan::PVOC, flan::Func1x1>( L, "stretch_spline" );
    luaF_register_helper<F_PVOC_desample, flan::PVOC, flan::PVOC, flan::Func2x1, flan::Func1x1>( L, "desample" );
    luaF_register_helper<F_PVOC_timeExtrapolate, flan::PVOC, flan::PVOC, flan::Time, flan::Time, flan::Time, flan::Func1x1>( L, "timeExtrapolate" );

    // Extras
    luaF_register_helper<F_PVOC_addOctaves, flan::PVOC, flan::PVOC, flan::Func2x1 >( L, "addOctaves" );
    luaF_register_helper<F_PVOC_addHarmonics, flan::PVOC, flan::PVOC, flan::Func2x1 >( L, "addHarmonics" );
    luaF_register_helper<F_PVOC_replaceAmplitudes, flan::PVOC, flan::PVOC, flan::PVOC, flan::Func2x1 >( L, "replaceAmplitudes" );
    luaF_register_helper<F_PVOC_subtractAmplitudes, flan::PVOC, flan::PVOC, flan::PVOC, flan::Func2x1 >( L, "subtractAmplitudes" );
    luaF_register_helper<F_PVOC_shape, flan::PVOC, flan::PVOC, flan::Func2x2 >( L, "shape" );
    luaF_register_helper<F_PVOC_perturb, flan::PVOC, flan::PVOC, flan::Func2x1, flan::Func2x1 >( L, "perturb" );
    luaF_register_helper<F_PVOC_retainNLoudestPartials, flan::PVOC, flan::PVOC, flan::Func1x1 >( L, "retainNLoudestPartials" );
    luaF_register_helper<F_PVOC_removeNLoudestPartials, flan::PVOC, flan::PVOC, flan::Func1x1 >( L, "removeNLoudestPartials" );
    luaF_register_helper<F_PVOC_resonate, flan::PVOC, flan::PVOC, flan::Time, flan::Func2x1>( L, "resonate" );

    // Static
    lua_pushcclosure( L, luaF_LTMP<F_PVOC_generate, PVOC, flan::Time, Func1x1, Func2x1>, 0 ); lua_setglobal( L, "generatePVOC" ); 

	lua_pop( L, 2 );
    }


	