#include "PV.h"

#include "PV.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <flan/Audio/Audio.h>
#include <flan/PV/PV.h>
#include <flan/Graph.h>

#include "LTMP.h"
#include "Usertypes.h"
#include "Interpolators.h"
#include "Function.h"

using namespace flan;

//============================================================================================================================================================
// Constructors
//============================================================================================================================================================

struct F_PV_create_null { pPV operator()()
    { std::cout << "flan::PV::create_null";
    return std::make_shared<flan::PV>( PV::create_null() ); } };

struct F_PV_load_from_file { pPV operator()( const std::string & filename )
    { std::cout << "flan::PV::load_from_file";
    return std::make_shared<flan::PV>( PV::load_from_file( filename ) ); } };


//============================================================================================================================================================
// Conversions
//============================================================================================================================================================

struct F_PV_convert_to_audio { pAudio operator()( pPV a )
    { std::cout << "flan::PV::convert_to_audio";
    return std::make_shared<flan::Audio>( a->convert_to_audio() ); } };

struct F_PV_convert_to_lr_audio { pAudio operator()( pPV a )
    { std::cout << "flan::PV::convert_to_lr_audio";
    return std::make_shared<flan::Audio>( a->convert_to_lr_audio() ); } };

struct F_Audio_convert_to_audio_selector { pAudio operator()( pPV a )
    { 
    if( a->get_num_channels() == 2 )
        return F_PV_convert_to_lr_audio()( a );
    else 
        return F_PV_convert_to_audio()( a );
    } };

// struct F_PV_convertToGraph { flan::Graph operator()( pPV a, Rect b = { 0, 0, -1, -1 }, Pixel c = -1, Pixel d = -1, float e=0 )
//     { std::cout << "flan::PV::";
//    return std::make_shared<flan::PV>( a->convertToGraph( b, c, d, e ); } };

struct F_PV_save_to_bmp { void operator()( pPV a, const std::string & b, Rect c = { 0, 0, -1, -1 }, Pixel d = -1, Pixel e = -1 )
    { std::cout << "flan::PV::save_to_bmp";
    a->save_to_bmp( b, c, d, e ); } };



//============================================================================================================================================================
// Contours
//============================================================================================================================================================

 struct F_PV_prism { pPV operator()( pPV a, 
    pPrismFunc b, 
    bool c = true )
    { std::cout << "flan::PV::";
    return std::make_shared<flan::PV>( a->prism( *b, c ) ); } };



//============================================================================================================================================================
// Utility
//============================================================================================================================================================

// struct F_PV_getFrame { pPV operator()( pPV a, flan::Second b = 0 )
//     { std::cout << "flan::PV::";
//    return std::make_shared<flan::PV>( a->getFrame( b ); } };



//============================================================================================================================================================
// Selection
//============================================================================================================================================================

struct F_PV_select { pPV operator()( pPV a, 
    flan::Second b = 5, 
    pFunc2x2 c = std::make_shared<Func2x2>( []( vec2 z ){ return z; } ) )
    { 
    std::cout << "flan::PV::select";
    return std::make_shared<flan::PV>( a->select( b, wrapFuncAxB<TF, TF>( c ) ) ); 
    } }; 

struct F_PV_freeze { pPV operator()( pPV a, 
    const std::vector<Second> & pause_times,
	const std::vector<Second> & pause_lengths )
    { std::cout << "flan::PV::";
   return std::make_shared<flan::PV>( a->freeze( pause_times, pause_lengths ) ); } };



//============================================================================================================================================================
// Resampling
//============================================================================================================================================================

struct F_PV_modify { pPV operator()( pPV a, 
    pFunc2x2 b, 
    InterpolatorIndex c = InterpolatorIndex::linear )
    { std::cout << "flan::PV::modify";
    return std::make_shared<flan::PV>( a->modify( wrapFuncAxB<TF, TF>( b ), index2interpolator( c ) ) ); } }; 

struct F_PV_modify_frequency { pPV operator()( pPV a, 
    pFunc2x1 b, 
    InterpolatorIndex c = InterpolatorIndex::linear )
    { std::cout << "flan::PV::modify_frequency";
    return std::make_shared<flan::PV>( a->modify_frequency( wrapFuncAxB<TF, Frequency>( b ), index2interpolator( c ) ) ); } };

struct F_PV_modify_time { pPV operator()( pPV a, 
    pFunc2x1 b, 
    InterpolatorIndex c = InterpolatorIndex::linear )
    { std::cout << "flan::PV::modify_time";
    return std::make_shared<flan::PV>( a->modify_time( wrapFuncAxB<TF, Second>( b ), index2interpolator( c ) ) ); } };

struct F_PV_repitch { pPV operator()( pPV a, 
    pFunc2x1 b, 
    InterpolatorIndex c = InterpolatorIndex::linear )
    { std::cout << "flan::PV::repitch";
    return std::make_shared<flan::PV>( a->repitch( wrapFuncAxB<TF, float>( b ), index2interpolator( c ) ) ); } };

struct F_PV_stretch { pPV operator()( pPV a, 
    pFunc2x1 b, 
    InterpolatorIndex c = InterpolatorIndex::linear )
    { std::cout << "flan::PV::stretch";
    return std::make_shared<flan::PV>( a->stretch( wrapFuncAxB<TF, float>( b ), index2interpolator( c ) ) ); } };

struct F_PV_stretch_spline { pPV operator()( pPV a, 
    pFunc1x1 b)
    { std::cout << "flan::PV::stretch_spline";
    return std::make_shared<flan::PV>( a->stretch_spline( *b ) ); } };

struct F_PV_desample { pPV operator()( pPV a, 
    pFunc2x1 b, 
    InterpolatorIndex c = InterpolatorIndex::linear )
    { std::cout << "flan::PV::desample";
    return std::make_shared<flan::PV>( a->desample( wrapFuncAxB<TF, float>( b ), index2interpolator( c ) ) ); } };

struct F_PV_time_extrapolate { pPV operator()( pPV a, 
    flan::Second b = 0, 
    flan::Second c = -1, 
    flan::Second d = 5, 
    InterpolatorIndex e = InterpolatorIndex::linear )
    { std::cout << "flan::PV::time_extrapolate";
    return std::make_shared<flan::PV>( a->time_extrapolate( b, c, d, index2interpolator( e ) ) ); } };



//============================================================================================================================================================
// Extras
//============================================================================================================================================================

struct F_PV_add_octaves { pPV operator()( pPV a, 
    pFunc2x1 b )
    { std::cout << "flan::PV::add_octaves";
    return std::make_shared<flan::PV>( a->add_octaves( wrapFuncAxB<std::pair<Second, Harmonic>, float>( b ) ) ); } };

struct F_PV_add_harmonics { pPV operator()( pPV a, 
    pFunc2x1 b )
    { std::cout << "flan::PV::add_harmonics";
    return std::make_shared<flan::PV>( a->add_harmonics( wrapFuncAxB<std::pair<Second, Harmonic>, float>( b ) ) ); } };

struct F_PV_replace_amplitudes { pPV operator()( pPV a, 
    pPV b, 
    pFunc2x1 c = std::make_shared<Func2x1>( 1 ) )
    { std::cout << "flan::PV::replace_amplitudes";
    return std::make_shared<flan::PV>( a->replace_amplitudes( *b, wrapFuncAxB<TF, float>( c ) ) ); } };

struct F_PV_subtract_amplitudes { pPV operator()( pPV a, 
    pPV b, 
    pFunc2x1 c = std::make_shared<Func2x1>( 1 ) )
    { std::cout << "flan::PV::subtract_amplitudes";
    return std::make_shared<flan::PV>( a->subtract_amplitudes( *b, wrapFuncAxB<TF, float>( c ) ) ); } };

struct F_PV_shape { pPV operator()( pPV a, 
    pFunc2x2 b )
    { std::cout << "flan::PV::shape";
    return std::make_shared<flan::PV>( a->shape( wrapFuncAxB<MF, MF>( b ) ) ); } };

// // struct F_PV_perturb { pPV operator()( pPV a, 
// //     pFunc2x2 b )
// //     { std::cout << "flan::PV::";
//    return std::make_shared<flan::PV>( a->perturb( wrapFuncAxB<TF, MF>( b ) ) ); } };

struct F_PV_retain_n_loudest_partials { pPV operator()( pPV a, 
    pFunc1x1 b )
    { std::cout << "flan::PV::retain_n_loudest_partials";
    return std::make_shared<flan::PV>( a->retain_n_loudest_partials( wrapFuncAxB<Second, Bin>( b ) ) ); } };

struct F_PV_remove_n_loudest_partials { pPV operator()( pPV a, 
    pFunc1x1 b )
    { std::cout << "flan::PV::remove_n_loudest_partials";
    return std::make_shared<flan::PV>( a->remove_n_loudest_partials( wrapFuncAxB<Second, Bin>( b ) ) ); } };

struct F_PV_resonate { pPV operator()( pPV a, 
    flan::Second b, 
    pFunc2x1 c )
    { std::cout << "flan::PV::resonate";
    return std::make_shared<flan::PV>( a->resonate( b, wrapFuncAxB<TF, float>( c ) ) ); } };



//============================================================================================================================================================
// Synthesis
//============================================================================================================================================================

struct F_PV_synthesize { pPV operator()( 
    flan::Second a = 5, 
    pFunc1x1 freq = std::make_shared<Func1x1>( 220 ), 
	pFunc2x1 harmonic_weights = std::make_shared<Func2x1>( []( vec2 sh ){ return 1.0f / ( 1.0f + sh.y() ); } ),
    pFunc1x1 harmonic_bandwidth = std::make_shared<Func1x1>( 60 ),
    pFunc2x1 harmonic_frequency_std_dev = std::make_shared<Func2x1>( 60 ) )
    { 
    std::cout << "flan::PV::synthesize";
    return std::make_shared<PV>( PV::synthesize( a, *freq, 
        wrapFuncAxB<std::pair<Second, Harmonic>, float>( harmonic_weights ), 
        *harmonic_bandwidth, 
        wrapFuncAxB<TF, Frequency>( harmonic_frequency_std_dev ) 
        ) ); 
    } };
 


//============================================================================================================================================================
// Registration
//============================================================================================================================================================

void luaF_register_PV( lua_State * L )
    {
    // Create PV type
    lua_newtable( L );
        lua_newtable( L );
        lua_setmetatable( L, -2 );
        lua_pushcclosure( L, luaF_LTMP<F_PV_create_null,    0>, 0 ); lua_setfield( L, -2, "create_null" ); 
        lua_pushcclosure( L, luaF_LTMP<F_PV_load_from_file, 1>, 0 ); lua_setfield( L, -2, "load_from_file" ); 
        lua_pushcclosure( L, luaF_LTMP<F_PV_synthesize,     2>, 0 ); lua_setfield( L, -2, "synthesize" ); 
    lua_setglobal( L, luaF_getUsertypeName<pPV>().c_str() );
	luaL_newmetatable( L, luaF_getUsertypeName<pPV>().c_str() );
    lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" ); // I need to look up why this works this way 

    // Create PV vec type
    lua_register( L, luaF_getUsertypeName<PVVec>().c_str(), luaF_Usertype_vec_new<pPV> );
	luaL_newmetatable( L, luaF_getUsertypeName<PVVec>().c_str() );
    lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );

    // | PV Metatable | PVVec Metatable

    // Conversions
    luaF_register_helper<F_PV_convert_to_audio,             1>( L, "convert_to_audio" );
    luaF_register_helper<F_PV_convert_to_lr_audio,          1>( L, "convert_to_lr_audio" );
    // luaF_register_helper<F_Audio_convert_to_audio_selector, 1>( L, "__call" );
    // luaF_register_helper<F_PV_convertToGraph, flan::Graph, PV, Rect, Pixel, Pixel, float>( L, "convertToGraph" );
    luaF_register_helper<F_PV_save_to_bmp,                  2>( L, "save_to_bmp" );

    // Contour
    luaF_register_helper<F_PV_prism,                        2>( L, "prism" );

    // Utility
    // luaF_register_helper<F_PV_getFrame,                 2>( L, "getFrame" );

    // Selection
    luaF_register_helper<F_PV_select,                       3>( L, "select" );
    luaF_register_helper<F_PV_freeze,                       3>( L, "freeze" );
    
    // Resampling
    luaF_register_helper<F_PV_modify,                       2>( L, "modify" );
    luaF_register_helper<F_PV_modify_frequency,             2>( L, "modify_frequency" );
    luaF_register_helper<F_PV_modify_time,                  2>( L, "modify_time" );
    luaF_register_helper<F_PV_repitch,                      2>( L, "repitch" );
    luaF_register_helper<F_PV_stretch,                      2>( L, "stretch" );
    luaF_register_helper<F_PV_stretch_spline,               2>( L, "stretch_spline" );
    luaF_register_helper<F_PV_desample,                     2>( L, "desample" );
    luaF_register_helper<F_PV_time_extrapolate,             4>( L, "time_extrapolate" );

    // Extras
    luaF_register_helper<F_PV_add_octaves,                  2>( L, "add_octaves" );
    luaF_register_helper<F_PV_add_harmonics,                2>( L, "add_harmonics" );
    luaF_register_helper<F_PV_replace_amplitudes,           2>( L, "replace_amplitudes" );
    luaF_register_helper<F_PV_subtract_amplitudes,          2>( L, "subtract_amplitudes" );
    luaF_register_helper<F_PV_shape,                        2>( L, "shape" );
    // luaF_register_helper<F_PV_perturb,                  3>( L, "perturb" );
    luaF_register_helper<F_PV_retain_n_loudest_partials,    2>( L, "retain_n_loudest_partials" );
    luaF_register_helper<F_PV_remove_n_loudest_partials,    2>( L, "remove_n_loudest_partials" );
    luaF_register_helper<F_PV_resonate,                     3>( L, "resonate" );

	lua_pop( L, 2 );
    }


	