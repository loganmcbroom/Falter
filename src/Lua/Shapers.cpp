#include "Shapers.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "Types.h"
#include "LTMP.h"
#include "Function.h"

#include <flan/Audio/distortion_defs.h>

using namespace flan;

struct F_Shapers_Function { pShaper operator()( pFunc2x1 a )
    { return std::make_shared<FunctionShaper>( wrapFuncAxB<std::pair<Second, Sample>, Sample>(a) ); } }; 

struct F_Shapers_Gain { pShaper operator()( pFunc1x1 a ) 
    { return std::make_shared<GainShaper>( *a ); } };

struct F_Shapers_SoftClip { pShaper operator()( pFunc1x1 a = std::make_shared<Func1x1>( 1 ) ) 
    { return std::make_shared<SoftClipShaper>( *a ); } };

struct F_Shapers_Rectifier { pShaper operator()( Fool a = true ) 
    { return std::make_shared<RectifierShaper>( a.b ); } };

struct F_Shapers_Quantize { pShaper operator()( pFunc1x1 a ) 
    { return std::make_shared<QuantizeShaper>( *a ); } };

struct F_Shapers_HardClip { pShaper operator()( pFunc1x1 a = std::make_shared<Func1x1>( 1 ) ) 
    { return std::make_shared<HardClipShaper>( *a ); } };

struct F_Shapers_DCOffset { pShaper operator()( pFunc1x1 a ) 
    { return std::make_shared<DCOffsetShaper>( *a ); } };

struct F_Shapers_SineFold { pShaper operator()( pFunc1x1 a ) 
    { return std::make_shared<SineFoldShaper>( *a ); } };

struct F_Shapers_Cubic { pShaper operator()( pFunc1x1 a ) 
    { return std::make_shared<CubicShaper>( *a ); } };

struct F_Shapers_Exponential { pShaper operator()( pFunc1x1 a ) 
    { return std::make_shared<ExponentialShaper>( *a ); } };

struct F_Shapers_Wavefolder { pShaper operator()( pFunc1x1 a ) 
    { return std::make_shared<WavefolderShaper>( *a ); } };

struct F_Shapers_Tremolo { pShaper operator()( pFunc1x1 a, pFunc1x1 b = std::make_shared<Func1x1>( 1.0f ) ) 
    { return std::make_shared<TremoloShaper>( *a, *b ); } };

struct F_Shapers_Downsample { pShaper operator()( pFunc1x1 a ) 
    { return std::make_shared<DownsampleShaper>( *a ); } };

struct F_Shapers_RingMod { pShaper operator()( pFunc1x1 a, pFunc1x1 b = std::make_shared<Func1x1>( 1.0f ) ) 
    { return std::make_shared<RingModShaper>( *a, *b ); } };

struct F_Shapers_Slew { pShaper operator()( pFunc1x1 a, pFunc1x1 b ) 
    { return std::make_shared<SlewShaper>( *a, *b ); } };

struct F_Shapers_Allpass { pShaper operator()( pFunc1x1 a, pFunc1x1 b, float c = 1.0f ) 
    { return std::make_shared<AllpassShaper>( *a, *b, c ); } };

struct F_Shapers_Comb { pShaper operator()( pFunc1x1 a, pFunc1x1 b = std::make_shared<Func1x1>( 0.0f ), float c = 1.0f ) 
    { return std::make_shared<CombShaper>( *a, *b, c ); } };

struct F_Shapers_EnvelopeFollower { pShaper operator()( pFunc1x1 a, pFunc1x1 b = std::make_shared<Func1x1>( 0.001f ), pFunc1x1 c = std::make_shared<Func1x1>( 1.0f ) ) 
    { return std::make_shared<EnvelopeFollowerShaper>( *a, *b, *c ); } };

struct F_Shapers_Compressor { pShaper operator()( 
    pFunc1x1 a, 
    pFunc1x1 b = std::make_shared<Func1x1>( 0.5f ), 
    pFunc1x1 c = std::make_shared<Func1x1>( 0.001f ),
    pFunc1x1 d = std::make_shared<Func1x1>( 0.1f ) )
    { return std::make_shared<CompressorShaper>( *a, *b, *c, *d ); } };

struct F_Shapers_Diode { pShaper operator()( 
    pFunc1x1 a, 
    pFunc1x1 b = std::make_shared<Func1x1>( 0.0f ) ) 
    { return std::make_shared<DiodeShaper>( *a, *b ); } };

struct F_Shapers_NoiseGate { pShaper operator()(
    pFunc1x1 a, 
    pFunc1x1 b = std::make_shared<Func1x1>( 0.0f ), 
    pFunc1x1 c = std::make_shared<Func1x1>( 0.001f ),
    pFunc1x1 d = std::make_shared<Func1x1>( 0.05f ) ) 
    { return std::make_shared<NoiseGateShaper>( *a, *b, *c, *d ); } };

struct F_Shapers_Lowpass { pShaper operator()( pFunc1x1 b, int a = 2 ) 
    { return std::make_shared<LowpassShaper>( a, *b ); } };

struct F_Shapers_Highpass { pShaper operator()( pFunc1x1 b, int a = 2 ) 
    { return std::make_shared<HighpassShaper>( a, *b ); } };

struct F_Shapers_Delay { pShaper operator()( pFunc1x1 a ) 
    { return std::make_shared<DelayShaper>( *a ); } };

struct F_Shapers_GranularPitch { pShaper operator()(
    pFunc1x1 a,
    pFunc1x1 b,
    float c = 0.1f,
    float d = 4.0f
    ) 
    { return std::make_shared<GranularPitchShaper>( *a, *b, c, d ); } };


void luaF_register_Shapers( lua_State * L )
    {
    luaL_newmetatable( L, luaF_getUsertypeName<pShaper>().c_str() );
    lua_pop( L, 1 );

    lua_newtable( L );
	    lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Function,         1>, 0 ); lua_setfield( L, -2, "func"       );
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Gain,             1>, 0 ); lua_setfield( L, -2, "gain"       );
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_SoftClip,         0>, 0 ); lua_setfield( L, -2, "soft"       );
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Rectifier,        0>, 0 ); lua_setfield( L, -2, "rectify"    );
	    lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Quantize,         1>, 0 ); lua_setfield( L, -2, "quantize"   );
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_HardClip,         0>, 0 ); lua_setfield( L, -2, "hard"       );
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_DCOffset,         1>, 0 ); lua_setfield( L, -2, "dc"         );        
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_SineFold,         1>, 0 ); lua_setfield( L, -2, "sinefold"   );        
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Cubic,            1>, 0 ); lua_setfield( L, -2, "cubic"      );        
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Exponential,      1>, 0 ); lua_setfield( L, -2, "exp"        );            
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Wavefolder,       1>, 0 ); lua_setfield( L, -2, "fold"       );            
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Tremolo,          1>, 0 ); lua_setfield( L, -2, "tremolo"    );        
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Downsample,       1>, 0 ); lua_setfield( L, -2, "downsample" );            
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_RingMod,          1>, 0 ); lua_setfield( L, -2, "ring"       );        
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Slew,             2>, 0 ); lua_setfield( L, -2, "slew"       );    
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Allpass,          2>, 0 ); lua_setfield( L, -2, "allpass"    );        
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Comb,             1>, 0 ); lua_setfield( L, -2, "comb"       );    
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_EnvelopeFollower, 1>, 0 ); lua_setfield( L, -2, "env"        );                
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Compressor,       1>, 0 ); lua_setfield( L, -2, "compress"   );            
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Diode,            1>, 0 ); lua_setfield( L, -2, "diode"      );        
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_NoiseGate,        1>, 0 ); lua_setfield( L, -2, "gate"       );            
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Lowpass,          1>, 0 ); lua_setfield( L, -2, "lowpass"    );        
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Highpass,         1>, 0 ); lua_setfield( L, -2, "highpass"   );        
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_Delay,            1>, 0 ); lua_setfield( L, -2, "delay"      );        
        lua_pushcclosure( L, luaF_LTMP_force_single_return<F_Shapers_GranularPitch,    2>, 0 ); lua_setfield( L, -2, "grain"      );                
    lua_setglobal( L, "sh" );
    }


	