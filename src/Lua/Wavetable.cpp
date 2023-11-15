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
    float d = .3, 
    Frame e = 256 )
    { 
    std::cout << "flan::Wavetable( Audio, SnapMode, PitchMode, snap_ratio, fixed_frame_size )";
    auto wt = std::make_shared<Wavetable>( *a, static_cast<Wavetable::SnapMode>( b ), static_cast<Wavetable::PitchMode>( c ), d, e ); 
    wt->remove_dc_in_place();
    wt->remove_jumps_in_place();
    return wt;
    } };

struct F_Wavetable_Function_Ctor { pWavetable operator()(
    pFunc1x1 a, 
    int b )
    { 
    std::cout << "flan::Wavetable( Function<Second, Amplitude>, num_waves )";
    auto wt = std::make_shared<Wavetable>( *a, b ); 
    return wt;
    } };

struct F_Wavetable_synthesize { pAudio operator()( pWavetable a, 
    flan::Second b, 
    pFunc1x1 c, 
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
    // Create Wavetable type
    lua_newtable( L );
        lua_newtable( L );
        lua_setmetatable( L, -2 );
        lua_pushcclosure( L, luaF_LTMP<F_Wavetable_Audio_Ctor,    1>, 0 ); lua_setfield( L, -2, "create_from_audio" ); 
        lua_pushcclosure( L, luaF_LTMP<F_Wavetable_Function_Ctor, 2>, 0 ); lua_setfield( L, -2, "create_from_function" ); 

    lua_setglobal( L, luaF_getUsertypeName<pWavetable>().c_str() );

	luaL_newmetatable( L, luaF_getUsertypeName<pWavetable>().c_str() );
        lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );
        // Create Wavetable vec type
        lua_register( L, luaF_getUsertypeName<WavetableVec>().c_str(), luaF_Usertype_vec_new<pWavetable> );
        luaL_newmetatable( L, luaF_getUsertypeName<WavetableVec>().c_str() );
            lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );

            luaF_register_helper<F_Wavetable_synthesize,            3>( L, "synthesize"            );
            luaF_register_helper<F_Wavetable_add_fades_in_place,    1>( L, "add_fades_in_place"    );
            luaF_register_helper<F_Wavetable_remove_jumps_in_place, 1>( L, "remove_jumps_in_place" );
            luaF_register_helper<F_Wavetable_remove_dc_in_place,    1>( L, "remove_dc_in_place"    ); 
   
    lua_pop( L, 2 );
    }