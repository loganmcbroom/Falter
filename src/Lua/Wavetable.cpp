#include "Wavetable.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <flan/Wavetable.h>

#include "Usertypes.h"
#include "Utility.h"
#include "LTMP.h"

using namespace flan;

template <typename E>
static constexpr typename std::underlying_type<E>::type to_underlying( E e ) 
    {
    return static_cast<typename std::underlying_type<E>::type>( e );
    }

struct F_Wavetable_Audio_Ctor { Wavetable operator()( std::atomic<bool> & z, Audio a, 
    std::underlying_type_t<Wavetable::SnapMode> b = to_underlying( Wavetable::SnapMode::Zero ), 
    std::underlying_type_t<Wavetable::PitchMode> c = to_underlying( Wavetable::PitchMode::Local ), 
    float d = .3, Frame e = 256 )
    { return Wavetable( a, static_cast<Wavetable::SnapMode>( b ), static_cast<Wavetable::PitchMode>( c ), d, e, z ); } };

struct F_Wavetable_Function_Ctor { Wavetable operator()( std::atomic<bool> & z, Func1x1 a, int b = 1 )
    { return Wavetable( a, b, z ); } };

static int luaF_Wavetable_ctor_selector( lua_State * L )
    {
    if( luaF_is<Audio>( L, 2 ) || luaF_is<AudioVec>( L, 2 ) )
        return luaF_LTMP<F_Wavetable_Audio_Ctor, 1>( L );
    else if( luaF_is<Func1x1>( L, 2 ) || luaF_is<Func1x1Vec>( L, 2 ) )
        return luaF_LTMP<F_Wavetable_Function_Ctor, 2>( L );
    else return luaL_error( L, "Wavetable couldn't be constructed with the given arguments." );
    }

struct F_Wavetable_synthesize { Audio operator()( std::atomic<bool> & z, Wavetable a, flan::Time b = 3, Func1x1 c = 220, Func1x1 d = 0, flan::Time e = 0.001f )
    { return a.synthesize( b, c, d, e, z ); } };
    
struct F_Wavetable_addFades { void operator()( std::atomic<bool> &, Wavetable a, Frame b = 32, Waveform c = -1 )
    { return a.addFades( b, c ); } };

struct F_Wavetable_removeJumps { void operator()( std::atomic<bool> &, Wavetable a, Frame b = 32, Waveform c = -1 )
    { return a.removeJumps( b, c ); } };

struct F_Wavetable_removeDC { void operator()( std::atomic<bool> &, Wavetable a, Waveform b = -1 )
    { return a.removeDC( b ); } };


void luaF_register_Wavetable( lua_State * L )
    {   
    lua_register( L, luaF_getUsertypeName<Wavetable>().c_str(), luaF_Wavetable_ctor_selector );
	luaL_newmetatable( L, luaF_getUsertypeName<Wavetable>().c_str() );
    lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" ); // I need to look up why this works this way

    lua_register( L, luaF_getUsertypeName<WavetableVec>().c_str(), luaF_Wavetable_ctor_selector );
	luaL_newmetatable( L, luaF_getUsertypeName<WavetableVec>().c_str() );
    lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );

    luaF_register_helper<F_Wavetable_synthesize,  4>( L, "synthesize"  );
    luaF_register_helper<F_Wavetable_addFades,    1>( L, "addFades"    );
    luaF_register_helper<F_Wavetable_removeJumps, 1>( L, "removeJumps" );
    luaF_register_helper<F_Wavetable_removeDC,    1>( L, "removeDC"    ); 
    }