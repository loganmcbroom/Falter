#include "Audio.h"

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
#include "Utility.h"
#include "Function.h"

using namespace flan;

// Default args
static flan::Func1x1 Func1x1_Id = []( float x ){ return x; };

// Methods ======================================================================================================================

struct F_Audio_string_ctor { Audio operator()( std::atomic<bool> &, const std::string & s )
    { return Audio( s ); } };
static int luaF_Audio_ctor_selector( lua_State * L )
    {
    // arg 1 is the Audio global table
    if( lua_gettop( L ) < 2 ) // Default ctor
        return luaF_Usertype_new<Audio>( L );
    else if( luaF_is<std::string>( L, 2 ) || luaF_isArrayOfType<std::string>( L, 2 ) )
        return luaF_LTMP<F_Audio_string_ctor, 1>( L );
    else return luaL_error( L, "Audio couldn't be constructed from the given arguments." );
    }

// Conversions
struct F_Audio_convertToGraph { flan::Graph operator()( std::atomic<bool> & z, flan::Audio a, Interval b = Interval( 0, -1 ), Pixel c = -1, Pixel d = -1, float e = 0 )
    { return a.convertToGraph( b, c, d, e, z ); } };
struct F_Audio_saveToBMP { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, const std::string & b = "audio.bmp", Interval c = Interval( 0, -1 ), Pixel d = -1, Pixel e = -1 )
    { return a.saveToBMP( b, c, d, e, z ); } };
struct F_Audio_convertToPVOC { flan::PVOC operator()( std::atomic<bool> & z, flan::Audio a, Frame b = 2048, Frame c = 128, Frame d = 4096 )
    { return a.convertToPVOC( b, c, d, nullptr, z ); } };
struct F_Audio_convertToFunction { flan::Func1x1 operator()( std::atomic<bool> & z, flan::Audio a, flan::Time b = .001f )
    { return a.convertToFunction( b, z  ); } };
struct F_Audio_convertToLeftRight { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a )
    { return a.convertToLeftRight( z ); } };
struct F_Audio_convertToStereo { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a )
    { return a.convertToStereo( z); } };
struct F_Audio_convertToMono { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a )
    { return a.convertToMono( z ); } };
struct F_Audio_convertToMidSide { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a )
    { return a.convertToMidSide( z ); } };

// Info
struct F_Audio_getTotalEnergy { float operator()( std::atomic<bool> & z, flan::Audio a )
    { return a.getTotalEnergy( z ); } };
struct F_Audio_getLocalWavelength { float operator()( std::atomic<bool> & z, flan::Audio a, Channel b = 0, Frame c = 0, Frame d = 2048 )
    { return a.getLocalWavelength( b, c, d, nullptr, z ); } };
struct F_Audio_getLocalWavelengths { std::vector<float> operator()( std::atomic<bool> & z, flan::Audio a, Channel b = 0, Frame c = 0, Frame d = 2048, Frame e = 128 )
    { return a.getLocalWavelengths( b, c, d, e, z ); } };
struct F_Audio_getAverageWavelength { float operator()( std::atomic<bool> & z, flan::Audio a, Channel b = 0, float c = 0, float d = -1, Frame e = 0, Frame f = -1, 
    Frame g = 2048, Frame h = 128 )
    { return a.getAverageWavelength( b, c, d, e, f, g, h, nullptr, z ); } };
struct F_Audio_getLocalFrequency { float operator()( std::atomic<bool> & z, flan::Audio a, Channel b = 0, Frame c = 0, Frame d = 2048 )
    { return a.getLocalFrequency( b, c, d, nullptr, z ); } };
struct F_Audio_getLocalFrequencies { std::vector<Frequency> operator()( std::atomic<bool> & z, flan::Audio a, Channel b = 0, Frame c = 0, Frame d = -1, Frame e = 2048, Frame f = 128 )
    { return a.getLocalFrequencies( b, c, d, e, f, nullptr, z ); } };

// Processing
struct F_Audio_invertPhase   { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a )
    { return a.invertPhase( z ); } };
struct F_Audio_modifyVolume  { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Func1x1 b = 1 )
    { return a.modifyVolume( b, z ); } };
struct F_Audio_setVolume     { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Func1x1 b = 1 )
    { return a.setVolume( b, z ); } };
struct F_Audio_shift         { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Time b = 0 )
    { return a.shift( b, z ); } };
struct F_Audio_waveshape     { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Func1x1 b = Func1x1_Id )
    { return a.waveshape( b, z ); } };
struct F_Audio_pan           { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Func1x1 b = 0 )
    { return a.pan( b, z ); } };
struct F_Audio_widen         { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Func1x1 b = 1 )
    { return a.widen( b, z ); } };
struct F_Audio_reverse       { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a )
    { return a.reverse( z ); } };
struct F_Audio_cut { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Time b = 0, flan::Time c = 0, flan::Time d = 0, flan::Time e = 0 )
    { return a.cut( b, c, d, e, z ); } };
struct F_Audio_cutFrames { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, Frame b = 0, Frame c = 0, Frame d = 0, Frame e = 0 )
    { return a.cutFrames( b, c, d, e, z ); } };
struct F_Audio_repitch { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Func1x1 b = 1, float c = .001, uint32_t d = 0 )
    { return a.repitch( b, c, d, z ); } };
// This method may need reformulation
// Audio convolve( const std::vector<Func1x1> & ir, flan_CANCEL_ARG ) const;
struct F_Audio_convolve { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Audio b = Audio() )
    { return a.convolve( b, z ); } };
struct F_Audio_fade { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Time b = 16.0f/48000.0f, flan::Time c = 16.0f/48000.0f, flan::Func1x1 d = flan::Interpolators::sqrt )
    { return a.fade( b, c, d, z ); } };
struct F_Audio_fadeFrames { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, Frame b = 16, Frame c = 16, flan::Func1x1 d = flan::Interpolators::sqrt )
    { return a.fadeFrames( b, c, d, z ); } };
// Leaving this for now until filter system is completed
// Audio lowPass( flan::Func1x1 cutoff, uint32_t taps = 64, flan_CANCEL_ARG ) const;
struct F_Audio_iterate { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, uint32_t b = 1, Audio::Mod c = nullptr, bool d = false )
    { return a.iterate( b, c, d, z ); } };
struct F_Audio_delay { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Time b = 10, flan::Func1x1 c = 1, flan::Func1x1 d = 1, Audio::Mod e = nullptr )
    { return a.delay( b, c, d, e, z ); } };
struct F_Audio_texture { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Time b = 10, flan::Func1x1 c = 1, flan::Func1x1 d = 0, Audio::Mod e = nullptr, bool f = false )
    { return a.texture( b, c, d, e, f, z ); } };
struct F_Audio_textureSimple { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Time b = 10, flan::Func1x1 c = 1, flan::Func1x1 d = 0, 
    flan::Func1x1 e = 1, flan::Func1x1 f = 1, flan::Func1x1 g = 1, flan::Func1x1 h = 0 )
    { return a.textureSimple( b, c, d, e, f, g, h, z ); } };
struct F_Audio_grainSelect { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Time b = 10, flan::Func1x1 c = 1, flan::Func1x1 d = 0, 
    flan::Func1x1 e = 0, flan::Func1x1 f = 0.1, flan::Func1x1 g = 0.05, Audio::Mod h = nullptr )
    { return a.grainSelect( b, c, d, e, f, g, h, z ); } };
struct F_Audio_chop { AudioVec operator()( std::atomic<bool> & z, flan::Audio a, flan::Time b = 1, flan::Time c = 0.05 )
    { return a.chop( b, c, z ); } };
struct F_Audio_rearrange { flan::Audio operator()( std::atomic<bool> & z, flan::Audio a, flan::Time b = 1, flan::Time c = 0.05 )
    { return a.rearrange( b, c, z ); } };


// Static
struct F_Audio_mix { flan::Audio operator()( std::atomic<bool> & z, AudioVec a, std::vector<Func1x1> b = std::vector<Func1x1>(), std::vector<float> c = std::vector<float>() )
    { return Audio::mix( a, b, c, z ); } };

struct F_Audio_join { flan::Audio operator()( std::atomic<bool> & z, AudioVec a, flan::Time b = 0 )
    { return Audio::join( a, b, z ); } };

struct F_Audio_select { flan::Audio operator()( std::atomic<bool> & z, AudioVec a, Func1x1 b = 0, std::vector<float> c = std::vector<float>() )
    { return Audio::select( a, b, c, z ); } };

struct F_Audio_synthesize { flan::Audio operator()( std::atomic<bool> & z, Func1x1 a, flan::Time b, Func1x1 c, int d = 48000, int e = 16 )
    { return Audio::synthesize( a, b, c, d, e, z ); } };

// Overloading  ====================================================================================================================

static int luaF_Audio_add( lua_State * L )
    {
    if( ! luaF_is<flan::Audio>( L, 2 ) ) return luaL_typerror( L, 2, luaF_getUsertypeName<flan::Audio>().c_str() );
    auto out = flan::Audio::mix( { luaF_check<flan::Audio>( L, 1 ), luaF_check<flan::Audio>( L, 2 ) } );
    luaF_push( L, out );
    return 1;
    }

static int luaF_Audio_concat( lua_State * L )
    {
    if( ! luaF_is<flan::Audio>( L, 2 ) ) return luaL_typerror( L, 2, luaF_getUsertypeName<flan::Audio>().c_str() );
    auto out = flan::Audio::join( { luaF_check<flan::Audio>( L, 1 ), luaF_check<flan::Audio>( L, 2 ) } );
    luaF_push( L, out );
    return 1;
    }


// Registration ==============================================================================================================

void luaF_register_Audio( lua_State * L )
    {
    // Create Lua Audio type
    lua_newtable( L ); // Global "Audio" table
        lua_newtable( L ); 
            lua_pushcfunction( L, luaF_Audio_ctor_selector ); lua_setfield( L, -2, "__call" );
        lua_setmetatable( L, -2 );
        lua_pushcclosure( L, luaF_LTMP<F_Audio_mix,         1>, 0 ); lua_setfield( L, -2, "mix" ); 
        lua_pushcclosure( L, luaF_LTMP<F_Audio_join,        1>, 0 ); lua_setfield( L, -2, "join" ); 
        lua_pushcclosure( L, luaF_LTMP<F_Audio_select,      2>, 0 ); lua_setfield( L, -2, "select" ); 
        lua_pushcclosure( L, luaF_LTMP<F_Audio_synthesize,  3>, 0 ); lua_setfield( L, -2, "synthesize" ); 
    lua_setglobal( L, luaF_getUsertypeName<flan::Audio>().c_str() );

	luaL_newmetatable( L, luaF_getUsertypeName<flan::Audio>().c_str() );
        lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" ); // I need to look up why this works this way
        lua_pushcfunction( L, luaF_Audio_add ); lua_setfield( L, -2, "__add" );
        lua_pushcfunction( L, luaF_Audio_concat ); lua_setfield( L, -2, "__concat" ); 

        // Create Lua AudioVec type
        lua_register( L, luaF_getUsertypeName<AudioVec>().c_str(), luaF_Usertype_vec_new<flan::Audio> );
	    luaL_newmetatable( L, luaF_getUsertypeName<AudioVec>().c_str() );
            lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );

            // Conversion
            //luaF_register_helper<F_Audio_convertToGraph, flan::Graph, flan::Audio, Interval, Pixel, Pixel, float>( L, "convertToGraph" );
            luaF_register_helper<F_Audio_saveToBMP,               2>( L, "saveToBMP" );
            luaF_register_helper<F_Audio_convertToPVOC,           1>( L, "convertToPVOC" );
            luaF_register_helper<F_Audio_convertToPVOC,           1>( L, "__call" );
            luaF_register_helper<F_Audio_convertToFunction,       1>( L, "convertToFunction" );
            luaF_register_helper<F_Audio_convertToLeftRight,      1>( L, "convertToLeftRight" );
            luaF_register_helper<F_Audio_convertToStereo,         1>( L, "convertToStereo" );
            luaF_register_helper<F_Audio_convertToMono,           1>( L, "convertToMono" );
            luaF_register_helper<F_Audio_convertToMidSide,        1>( L, "convertToMidSide" );

            // Info
            luaF_register_helper<F_Audio_getTotalEnergy,          1>( L, "getTotalEnergy" );
            luaF_register_helper<F_Audio_getLocalWavelength,      3>( L, "getLocalWavelength" );
            luaF_register_helper<F_Audio_getLocalWavelengths,     2>( L, "getLocalWavelengths" );
            luaF_register_helper<F_Audio_getAverageWavelength,    2>( L, "getAverageWavelength" );
            luaF_register_helper<F_Audio_getLocalFrequency,       2>( L, "getLocalFrequency" );
            luaF_register_helper<F_Audio_getLocalFrequencies,     2>( L, "getLocalFrequencies" );

            // Processing
            luaF_register_helper<F_Audio_invertPhase,             1>( L, "invertPhase" );
            luaF_register_helper<F_Audio_modifyVolume,            2>( L, "modifyVolume" );
            luaF_register_helper<F_Audio_setVolume,               2>( L, "setVolume" );
            luaF_register_helper<F_Audio_shift,                   2>( L, "shift" );
            luaF_register_helper<F_Audio_waveshape,               2>( L, "waveshape" );
            luaF_register_helper<F_Audio_pan,                     2>( L, "pan" );
            luaF_register_helper<F_Audio_widen,                   2>( L, "widen" );
            luaF_register_helper<F_Audio_reverse,                 1>( L, "reverse" );
            luaF_register_helper<F_Audio_cut,                     3>( L, "cut" );
            luaF_register_helper<F_Audio_cutFrames,               3>( L, "cutFrames" );
            luaF_register_helper<F_Audio_repitch,                 2>( L, "repitch" );
            luaF_register_helper<F_Audio_convolve,                2>( L, "convolve" );
            luaF_register_helper<F_Audio_fade,                    1>( L, "fade" );
            luaF_register_helper<F_Audio_fadeFrames,              1>( L, "fadeFrames" );
            // // Audio lowPass( flan::Func1x1 cutoff, uint32_t taps = 64, flan_CANCEL_ARG ) const;
            luaF_register_helper<F_Audio_iterate,                 2>( L, "iterate" );
            luaF_register_helper<F_Audio_delay,                   3>( L, "delay" );
            luaF_register_helper<F_Audio_texture,                 3>( L, "texture" );
            luaF_register_helper<F_Audio_textureSimple,           7>( L, "textureSimple" );
            luaF_register_helper<F_Audio_grainSelect,             5>( L, "grainSelect" );
            luaF_register_helper<F_Audio_chop,                    2>( L, "chop" );
            luaF_register_helper<F_Audio_rearrange,               2>( L, "rearrange" );

            // Static methods taking vector<Audio> are also added as AudioVec methods for convenience
            lua_pushcclosure( L, luaF_LTMP<F_Audio_mix, 1>, 0 ); lua_setfield( L, -2, "mix" ); 
            lua_pushcclosure( L, luaF_LTMP<F_Audio_join, 1>, 0 ); lua_setfield( L, -2, "join" ); 
            lua_pushcclosure( L, luaF_LTMP<F_Audio_select, 2>, 0 ); lua_setfield( L, -2, "select" ); 

	lua_pop( L, 2 );
    }
