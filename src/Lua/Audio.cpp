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

#include "AltarThread.h"

#include "Utility.h"

#define LUA_FLAN_AUDIO "Audio"
#define LUA_FLAN_AUDIO_VEC "AudioVec"

using namespace flan;

// Interface ===============================================================================================================================

bool luaF_isAudio( lua_State * L, int i )
    {
    return luaF_isUserType( L, i, LUA_FLAN_AUDIO );
    }

s_Audio luaF_checkAudio( lua_State * L, int i )
    {
    void* inputP = luaL_checkudata( L, i, LUA_FLAN_AUDIO );
    if( ! inputP ) luaL_typerror( L, i, LUA_FLAN_AUDIO );
    s_Audio * sap = static_cast<s_Audio*>( inputP );
    return *sap;
    }

void luaF_pushAudio( lua_State * L, s_Audio audio )
    {
    // Create output space
    void* outputP = lua_newuserdata( L, sizeof( s_Audio ) );
    if( ! outputP ) luaL_error( L, "Audio userdata couldn't be constructed." );

    // Construct s_Audio into created space, managing a new Audio which is move constructed from the method output
    new(outputP) s_Audio( audio );

    // Make the userdata a Lua Audio
    luaL_setmetatable( L, LUA_FLAN_AUDIO );
    }

// Vector versions

bool luaF_isAudioVec( lua_State * L, int i )
    {
    return luaF_isUserType( L, i, LUA_FLAN_AUDIO_VEC );
    }

std::vector<s_Audio> luaF_checkAudioVec( lua_State * L, int i )
    {
    if( ! luaF_isAudioVec( L, i ) )
        {
        luaL_error( L, "AudioVec expected but not recieved." );
        return {};
        }
    
    const int numInputs = lua_objlen( L, i );
    std::vector<s_Audio> outputs;
    for( int n = 1; n <= numInputs; ++n )
        {
        lua_rawgeti( L, i, n );
        outputs.push_back( luaF_checkAudio( L, -1 ) );
        }
    return outputs;   
    }

void luaF_pushAudioVec( lua_State * L, const std::vector<s_Audio> & audios )
    {
    lua_createtable( L, audios.size(), 0 );

    for( int i = 0; i < audios.size(); ++i )
        {
        luaF_pushAudio( L, audios[i] );
        lua_rawseti( L, -2, i + 1 );
        }

    // Make the table a Lua Audio vec
    luaL_setmetatable( L, LUA_FLAN_AUDIO_VEC );
    }

// Helpers

std::vector<s_Audio> luaF_checkAudioAsVec( lua_State * L, int i )
    {
    if( luaF_isAudio( L, i ) )
        {
        return { luaF_checkAudio( L, i ) };
        }
    else if( luaF_isAudioVec( L, i ) )
        {
        return luaF_checkAudioVec( L, i );
        }
    else return {};
    }


// Metamethods ==============================================================================================================

static int luaF_audio_new( lua_State* L )
    {
    // Allocate Lua space for the shared_ptr
    void* userData = lua_newuserdata( L, sizeof( s_Audio ) );
    if( ! userData ) return 0;

    // Construct shared_ptr into the space
    new(userData) s_Audio;

    luaL_setmetatable( L, LUA_FLAN_AUDIO );
    return 1;
    }

static int luaF_audio_delete( lua_State * L )
    {
	void* p = luaL_checkudata( L, 1, LUA_FLAN_AUDIO );
    if( p ) 
        {
        auto resource = static_cast<s_Audio*>( p );
        resource->reset();
        }
    return 0;
    }

static int luaF_audio_vec_new( lua_State* L )
    {
    lua_createtable( L, 0, 0 );
    luaL_setmetatable( L, LUA_FLAN_AUDIO_VEC );
    return 1;
    }


// Methods ======================================================================================================================

// Audio operator+( const Audio & other ) const;
// Audio operator-() const;
// Audio operator-( const Audio & other ) const;
// Sample getSampleInterpolated( uint32_t channel, float frame, Interpolator interp = Interpolators::linear ) const;

// Conversions
struct F_Audio_convertToGraph { Graph operator()( s_Audio a, Interval b = Interval( 0, -1 ), Pixel c = -1, Pixel d = -1, float e = 0 )
    { return a->convertToGraph( b, c, d, e ); } };
struct F_Audio_saveToBMP { s_Audio operator()( s_Audio a, const std::string & b = "audio.bmp", Interval c = Interval( 0, -1 ), Pixel d = -1, Pixel e = -1 )
    { return std::make_shared<Audio>( a->saveToBMP( b, c, d, e ) ); } };
struct F_Audio_convertToPVOC { s_PVOC operator()( s_Audio a, Frame b = 2048, Frame c = 128, Frame d = 4096 )
    { return std::make_shared<flan::PVOC>( a->convertToPVOC( b, c, d ) ); } };
struct F_Audio_convertToFunction { s_Func1x1 operator()( s_Audio a, flan::Time b = .001f )
    { return std::make_shared<flan::Func1x1>( a->convertToFunction( b ) ); } };
struct F_Audio_convertToLeftRight { s_Audio operator()( s_Audio a )
    { return std::make_shared<Audio>( a->convertToLeftRight() ); } };
struct F_Audio_convertToStereo { s_Audio operator()( s_Audio a )
    { return std::make_shared<Audio>( a->convertToStereo() ); } };
struct F_Audio_convertToMono { s_Audio operator()( s_Audio a )
    { return std::make_shared<Audio>( a->convertToMono() ); } };
struct F_Audio_convertToMidSide { s_Audio operator()( s_Audio a )
    { return std::make_shared<Audio>( a->convertToMidSide() ); } };

// Info
struct F_Audio_getTotalEnergy { float operator()( s_Audio a ){ return a->getTotalEnergy(); } };
struct F_Audio_getLocalWavelength { float operator()( s_Audio a, Channel b = 0, Frame c = 0, Frame d = 2048 )
    { return a->getLocalWavelength( b, c, d ); } };
struct F_Audio_getLocalWavelengths { std::vector<float> operator()( s_Audio a, Channel b = 0, Frame c = 0, Frame d = 2048, Frame e = 128 )
    { return a->getLocalWavelengths( b, c, d, e ); } };
struct F_Audio_getAverageWavelength { float operator()(  s_Audio a, Channel b = 0, float c = 0, float d = -1, Frame e = 0, Frame f = -1, 
    Frame g = 2048, Frame h = 128 )
    { return a->getAverageWavelength( b, c, d, e, f, g, h ); } };
struct F_Audio_getLocalFrequency { float operator()( s_Audio a, Channel b = 0, Frame c = 0, Frame d = 2048 )
    { return a->getLocalFrequency( b, c, d ); } };
struct F_Audio_getLocalFrequencies { std::vector<Frequency> operator()( s_Audio a, Channel b = 0, Frame c = 0, Frame d = -1, Frame e = 2048, Frame f = 128 )
    { return a->getLocalFrequencies( b, c, d, e, f ); } };

// Processing
struct F_Audio_invertPhase   { s_Audio operator()( s_Audio a )
    { return std::make_shared<Audio>( a->invertPhase() ); } };
struct F_Audio_modifyVolume  { s_Audio operator()( s_Audio a, s_Func1x1 b = std::make_shared<Func1x1>( 1 ) )
    { return std::make_shared<Audio>( a->modifyVolume( *b ) ); } };
struct F_Audio_setVolume     { s_Audio operator()( s_Audio a, s_Func1x1 b = std::make_shared<Func1x1>( 1 ) )
    { return std::make_shared<Audio>( a->setVolume( *b ) ); } };
struct F_Audio_shift         { s_Audio operator()( s_Audio a, flan::Time b = 0 )
    { return std::make_shared<Audio>( a->shift( b ) ); } };
struct F_Audio_waveshape     { s_Audio operator()( s_Audio a, s_Func1x1 b = std::make_shared<Func1x1>( []( float x ){ return x; } ) )
    { return std::make_shared<Audio>( a->waveshape( *b ) ); } };
struct F_Audio_pan           { s_Audio operator()( s_Audio a, s_Func1x1 b = std::make_shared<Func1x1>( 0 ) )
    { return std::make_shared<Audio>( a->pan( *b ) ); } };
struct F_Audio_widen         { s_Audio operator()( s_Audio a, s_Func1x1 b = std::make_shared<Func1x1>( 1 ) )
    { return std::make_shared<Audio>( a->widen( *b ) ); } };
struct F_Audio_reverse       { s_Audio operator()( s_Audio a )
    { return std::make_shared<Audio>( a->reverse() ); } };
struct F_Audio_cut { s_Audio operator()( s_Audio a, flan::Time b = 0, flan::Time c = 0, flan::Time d = 0, flan::Time e = 0 )
    { return std::make_shared<Audio>( a->cut( b, c, d, e ) ); } };
struct F_Audio_cutFrames { s_Audio operator()( s_Audio a, Frame b = 0, Frame c = 0, Frame d = 0, Frame e = 0 )
    { return std::make_shared<Audio>( a->cutFrames( b, c, d, e ) ); } };
struct F_Audio_repitch { s_Audio operator()( s_Audio a, s_Func1x1 b = std::make_shared<Func1x1>( 1 ), float c = .001, uint32_t d = 0 )
    { return std::make_shared<Audio>( a->repitch( *b, c, d ) ); } };

// This method may need reformulation
// Audio convolve( const std::vector<Func1x1> & ir, flan_CANCEL_ARG ) const;

struct F_Audio_convolve { s_Audio operator()( s_Audio a, s_Audio b = s_Audio( new Audio() ) )
    { return std::make_shared<Audio>( a->convolve( *b ) ); } };
struct F_Audio_fade { s_Audio operator()( s_Audio a, flan::Time b = 16.0f/48000.0f, flan::Time c = 16.0f/48000.0f, Interpolator d = flan::Interpolators::sqrt )
    { return std::make_shared<Audio>( a->fade( b, c, d ) ); } };
struct F_Audio_fadeFrames { s_Audio operator()( s_Audio a, Frame b = 16, Frame c = 16, Interpolator d = flan::Interpolators::sqrt )
    { return std::make_shared<Audio>( a->fadeFrames( b, c, d ) ); } };

// Leaving this for now until filter system is completed
// Audio lowPass( Func1x1 cutoff, uint32_t taps = 64, flan_CANCEL_ARG ) const;

struct F_Audio_iterate { s_Audio operator()( s_Audio a, uint32_t b = 1, Audio::Mod c = nullptr, bool d = false )
    { return std::make_shared<Audio>( a->iterate( b, c, d ) ); } };
struct F_Audio_delay { s_Audio operator()( s_Audio a, flan::Time b = 10, Func1x1 c = 1, Func1x1 d = .5, Audio::Mod e = nullptr )
    { return std::make_shared<Audio>( a->delay( b, c, d, e ) ); } };
struct F_Audio_texture { s_Audio operator()( s_Audio a, flan::Time b = 10, Func1x1 c = 1, Func1x1 d = 0, Audio::Mod e = nullptr, bool f = false )
    { return std::make_shared<Audio>( a->texture( b, c, d, e, f ) ); } };
struct F_Audio_textureSimple { s_Audio operator()( s_Audio a, flan::Time b = 10, Func1x1 c = 1, Func1x1 d = 0, Func1x1 e = 1, 
    Func1x1 f = 1, Func1x1 g = 1, Func1x1 h = 0 )
    { return std::make_shared<Audio>( a->textureSimple( b, c, d, e, f, g, h ) ); } };
struct F_Audio_grainSelect { s_Audio operator()( s_Audio a, flan::Time b = 10, Func1x1 c = 5, Func1x1 d = 0, Func1x1 e = 0, 
    Func1x1 f = 0.1, Func1x1 g = 0.05, Audio::Mod h = nullptr )
    { return std::make_shared<Audio>( a->grainSelect( b, c, d, e, f, g, h ) ); } };
//struct F_Audio_chop { AudioVec operator()( s_Audio a, flan::Time b = 1, flan::Time c = 0.05 )
//    { return a->chop( b, c ); } };
struct F_Audio_rearrange { s_Audio operator()( s_Audio a, flan::Time b = 1, flan::Time c = 0.05 )
    { return std::make_shared<Audio>( a->rearrange( b, c ) ); } };


// Multi-Input
// struct F_Audio_mix { Audio operator()( AudioVec a, 
//     std::vector<Func1x1> b = std::vector<Func1x1>(),
//     std::vector<flan::Time> c = std::vector<flan::Time>() )
//     { return Audio::mix( a, b, c ); } };
// struct F_Audio_join { Audio operator()( AudioVec a, flan::Time b = 0 ){ return Audio::join( a, b  ); } };
// struct F_Audio_select { Audio operator()( AudioVec a, Func1x1 b, std::vector<Time> c = std::vector<Time>())
//     { return Audio::select( a, b, c ); } };


// Registration ==============================================================================================================

template<typename ... Args>
void luaF_register_helper( lua_State* L, const std::string & name )
    {
    lua_pushcclosure( L, luaF_LTMP<Args...>, 0 );  lua_setfield( L, -3, name.c_str() ); 
    lua_pushcclosure( L, luaF_LTMP<Args...>, 0 );  lua_setfield( L, -2, name.c_str() );
    }

void luaF_register_Audio( lua_State * L )
    {
    // Create LUA_FLAN_AUDIO type
    lua_register( L, LUA_FLAN_AUDIO, luaF_audio_new );
	luaL_newmetatable( L, LUA_FLAN_AUDIO );
	lua_pushcfunction( L, luaF_audio_delete ); lua_setfield( L, -2, "__gc" );
    lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" ); // I need to look up why this works this way

    // Create LUA_FLAN_AUDIO_VEC type
    lua_register( L, LUA_FLAN_AUDIO_VEC, luaF_audio_vec_new );
	luaL_newmetatable( L, LUA_FLAN_AUDIO_VEC );
    lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );

    // | Audio Metatable | AudioVec Metatable

    // Conversion
    //luaF_register_helper<F_Audio_convertToGraph, Graph, s_Audio, Interval, Pixel, Pixel, float>( L, "convertToGraph" );
    //luaF_register_helper<F_Audio_saveToBMP, s_Audio, s_Audio, const std::string &, Interval, Pixel, Pixel>( L, "saveToBMP" );
    //luaF_register_helper<F_Audio_convertToPVOC, PVOC, s_Audio, Frame, Frame, Frame>( L, "convertToPVOC" );
    //luaF_register_helper<F_Audio_convertToFunction, Func1x1, s_Audio, flan::Time>( L, "convertToFunction" );
    luaF_register_helper<F_Audio_convertToLeftRight, s_Audio, s_Audio>( L, "convertToLeftRight" );
    luaF_register_helper<F_Audio_convertToStereo, s_Audio, s_Audio>( L, "convertToStereo" );
    luaF_register_helper<F_Audio_convertToMono, s_Audio, s_Audio>( L, "convertToMono" );
    luaF_register_helper<F_Audio_convertToMidSide, s_Audio, s_Audio>( L, "convertToMidSide" );

    // Info
    luaF_register_helper<F_Audio_getTotalEnergy, float, s_Audio>( L, "getTotalEnergy" );
    luaF_register_helper<F_Audio_getLocalWavelength, float, s_Audio, Channel, Frame, Frame>( L, "getLocalWavelength" );
    //luaF_register_helper<F_Audio_getLocalWavelengths, std::vector<float>, s_Audio, Channel, Frame, Frame, Frame>( L, "getLocalWavelengths" );
    luaF_register_helper<F_Audio_getAverageWavelength, float, s_Audio, Channel, float, float, Frame, Frame, Frame, Frame>( L, "getAverageWavelength" );
    luaF_register_helper<F_Audio_getLocalFrequency, float, s_Audio, Channel, Frame, Frame>( L, "getLocalFrequency" );
    //luaF_register_helper<F_Audio_getLocalFrequencies, std::vector<Frequency>, s_Audio, Channel, Frame, Frame, Frame, Frame>( L, "getLocalFrequencies" );

    // Processing
    luaF_register_helper<F_Audio_invertPhase, s_Audio, s_Audio>( L, "invertPhase" );
    luaF_register_helper<F_Audio_modifyVolume, s_Audio, s_Audio, s_Func1x1>( L, "modifyVolume" );
    luaF_register_helper<F_Audio_setVolume, s_Audio, s_Audio, s_Func1x1>( L, "setVolume" );
    luaF_register_helper<F_Audio_shift, s_Audio, s_Audio, flan::Time>( L, "shift" );
    luaF_register_helper<F_Audio_waveshape, s_Audio, s_Audio, s_Func1x1>( L, "waveshape" );
    luaF_register_helper<F_Audio_pan, s_Audio, s_Audio, s_Func1x1>( L, "pan" );
    luaF_register_helper<F_Audio_widen, s_Audio, s_Audio, s_Func1x1>( L, "widen" );
    luaF_register_helper<F_Audio_reverse, s_Audio, s_Audio>( L, "reverse" );
    luaF_register_helper<F_Audio_cut, s_Audio, s_Audio, flan::Time, flan::Time, flan::Time, flan::Time>( L, "cut" );
    luaF_register_helper<F_Audio_cutFrames, s_Audio, s_Audio, Frame, Frame, Frame, Frame>( L, "cutFrames" );
    luaF_register_helper<F_Audio_repitch, s_Audio, s_Audio, s_Func1x1, flan::Time, uint32_t>( L, "repitch" );

    // // luaF_register_helper<F_Audio_convolve>( L, "convolve" );
    // // luaF_register_helper<F_Audio_convolve>( L, "convolve" );
    // luaF_register_helper<F_Audio_fade>( L, "fade" );
    // luaF_register_helper<F_Audio_fadeFrames>( L, "fadeFrames" );
    // // Audio lowPass( Func1x1 cutoff, uint32_t taps = 64, flan_CANCEL_ARG ) const;
    // luaF_register_helper<F_Audio_iterate>( L, "iterate" );
    // luaF_register_helper<F_Audio_delay>( L, "delay" );
    // luaF_register_helper<F_Audio_texture>( L, "texture" );
    // luaF_register_helper<F_Audio_textureSimple>( L, "textureSimple" );
    // luaF_register_helper<F_Audio_grainSelect>( L, "grainSelect" );
    // //luaF_register_helper<F_Audio_chop>( L, "chop" );
    // luaF_register_helper<F_Audio_rearrange>( L, "rearrange" );

    // Multi-Input
    // luaF_register_helper<F_Audio_mix>( L, "fadeFrames" );
    // luaF_register_helper<F_Audio_join>( L, "fadeFrames" );
    // luaF_register_helper<F_Audio_select>( L, "fadeFrames" );


	lua_pop( L, 2 );
    }


	