#pragma once

#include <flan/Utility/Interpolator.h>

struct lua_State;

enum class InterpolatorIndex
	{
	midpoint,
    nearest,
    floor,
    ceil,
    linear,
    smoothstep,
    smootherstep,
    sine,
    sqrt 
	};

flan::Interpolator index2interpolator( InterpolatorIndex );
flan::Interpolator underlying2Interpolator( std::underlying_type_t<InterpolatorIndex> );

void luaF_register_Interpolators( lua_State * L );