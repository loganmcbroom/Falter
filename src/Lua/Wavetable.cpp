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

struct F_Wavetable_Audio_Ctor { pWavetable operator()( pAudio a, 
    std::underlying_type_t<Wavetable::SnapMode> b = to_underlying( Wavetable::SnapMode::Zero ), 
    std::underlying_type_t<Wavetable::PitchMode> c = to_underlying( Wavetable::PitchMode::Local ), 
    float d = .3, Frame e = 256 )
    { 
    std::cout << "flan::Wavetable( Audio, SnapMode, PitchMode, snap_ratio, fixed_frame_size )";
    auto wt = std::make_shared<Wavetable>( *a, static_cast<Wavetable::SnapMode>( b ), static_cast<Wavetable::PitchMode>( c ), d, e ); 
    wt->remove_dc_in_place();
    wt->remove_jumps_in_place();
    return wt;
    } };

struct F_Wavetable_Function_Ctor { pWavetable operator()(
    pFunc1x1 a, 
    int b = 1 )
    { 
    std::cout << "flan::Wavetable( Function<Second, Amplitude>, num_waves )";
    auto wt = std::make_shared<Wavetable>( *a, b ); 
    wt->remove_dc_in_place();
    wt->remove_jumps_in_place();
    return wt;
    } };

static int luaF_Wavetable_ctor_selector( lua_State * L )
    {
    if( luaF_is<pAudio>( L, 2 ) || luaF_is<AudioVec>( L, 2 ) )
        return luaF_LTMP<F_Wavetable_Audio_Ctor, 1>( L );
    else if( luaF_is<pFunc1x1>( L, 2 ) || luaF_is<Func1x1Vec>( L, 2 ) )
        return luaF_LTMP<F_Wavetable_Function_Ctor, 2>( L );
    else return luaL_error( L, "Wavetable couldn't be constructed with the given arguments." );
    }

struct F_Wavetable_synthesize { pAudio operator()( pWavetable a, 
    flan::Second b = 3, 
    pFunc1x1 c = std::make_shared<Func1x1>( 220 ), 
    pFunc1x1 d = std::make_shared<Func1x1>( 0 ), 
    flan::Second e = 0.001f 
    )
    { std::cout << "flan::Wavetable::synthesize";
    return std::make_shared<flan::Audio>( a->synthesize( b, *c, *d, e ) ); } };
    
struct F_Wavetable_add_fades_in_place { void operator()( pWavetable a, 
    Frame b = 32, 
    Waveform c = -1 
    )
    { std::cout << "flan::Wavetable::add_fades_in_place";
    a->add_fades_in_place( b, c ); } };

struct F_Wavetable_remove_jumps_in_place { void operator()( pWavetable a, 
    Frame b = 32, 
    Waveform c = -1 
    )
    { std::cout << "flan::Wavetable::remove_jumps_in_place";
    a->remove_jumps_in_place( b, c ); } };

struct F_Wavetable_remove_dc_in_place { void operator()( pWavetable a, 
    Waveform b = -1 )
    { std::cout << "flan::Wavetable::remove_dc_in_place";
    a->remove_dc_in_place( b ); } };


void luaF_register_Wavetable( lua_State * L )
    {   
    lua_register( L, luaF_getUsertypeName<pWavetable>().c_str(), luaF_Wavetable_ctor_selector );
	luaL_newmetatable( L, luaF_getUsertypeName<pWavetable>().c_str() );
    lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" ); // I need to look up why this works this way

    lua_register( L, luaF_getUsertypeName<WavetableVec>().c_str(), luaF_Wavetable_ctor_selector );
	luaL_newmetatable( L, luaF_getUsertypeName<WavetableVec>().c_str() );
    lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );

    luaF_register_helper<F_Wavetable_synthesize,            4>( L, "synthesize"            );
    // luaF_register_helper<F_Wavetable_add_fades_in_place,    1>( L, "add_fades_in_place"    );
    // luaF_register_helper<F_Wavetable_remove_jumps_in_place, 1>( L, "remove_jumps_in_place" );
    // luaF_register_helper<F_Wavetable_remove_dc_in_place,    1>( L, "remove_dc_in_place"    ); 
    }