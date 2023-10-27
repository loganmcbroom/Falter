#pragma once 

#include <flan/Function.h>

#include "../Types.h"

struct lua_State;

// Little util for passing between functions using isomorphic types
template<typename I, typename O, typename F>
flan::Function<I, O> wrapFuncAxB( const std::shared_ptr<F> & spf )
    {
    return flan::Function<I, O>( [spf]( I in )
        {
        using FI = F::ArgType;
        const auto fi = static_cast<FI>( in );
        const auto fo = (*spf)( fi );
        const auto o = static_cast<O>( fo );
        return o;
        }, spf->get_execution_policy() );
    }

template<typename T> T luaF_checkFunc( lua_State * L, int i );
template<typename T> bool luaF_isFunc( lua_State * L, int i );

void luaF_register_function_types( lua_State* L );