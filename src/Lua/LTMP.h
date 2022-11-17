#pragma once

#include <vector>
#include <functional>
#include <type_traits>

#include <JuceHeader.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "./FalterThread.h"
#include "./Types.h"

#include "Utility.h"

template<typename T> std::vector<T> luaF_LTMP_check( lua_State * L, int i )
    {
    if( luaF_is<T>( L, i ) )
        return { luaF_check<T>( L, i ) };
    else if( luaF_isArrayOfType<T>( L, i ) )
        return luaF_checkArrayOfType<T>( L, i );
    else 
        {
        luaL_error( L, ( std::string( "Expected type " ) + typeid( T ).name() ).c_str() );
        return {};
        }
    }
template<typename T> void luaF_LTMP_push( lua_State* L, const std::vector<T> & os )
    {
    if( os.size() == 0 ) 
        luaL_error( L, "luaF_LTMP_push was called with an empty output vector" );
    if( os.size() == 1 ) 
        luaF_push( L, os[0] );
    else
        luaF_pushArrayOfType( L, os );
    }


// getMaxSize - Calls size() on each input and returns the largest result
template<typename T>
static int getMaxSize( const T & t )
    {
    return t.size();
    }
template<typename T, typename ... Rest>
static int getMaxSize( const T & t, const Rest & ... rest )
    {
    return std::max( (size_t) t.size(), (size_t) getMaxSize( rest... ) );
    }


// getMinSize - Calls size() on each input and returns the smallest result
template<typename T>
static int getMinSize( const T & t )
    {
    return t.size();
    }
template<typename T, typename ... Rest>
static int getMinSize( const T & t, const Rest & ... rest )
    {
    return std::min( (size_t) t.size(), (size_t) getMinSize( rest... ) );
    }





// LTMP ===============================================================================================================================

template<typename Functor, size_t numArgs, size_t I = 0> 
constexpr auto luaF_wrapFunctor_checkArgs( lua_State * L ) 
    {
    if constexpr ( I == numArgs )
        return std::tuple();
    else  
        {
        // You can't get a reference from lua, so remove_ref and const
        using ArgType = std::remove_const_t< std::remove_reference_t< 
            liph::function_argument_n<I + 1, Functor> > >; // +1 to skip functor cancel arg
        return std::tuple_cat( 
            std::make_tuple( luaF_LTMP_check<ArgType>( L, I + 1 ) ), // +1 for lua indexing
            luaF_wrapFunctor_checkArgs<Functor, numArgs, I + 1>( L ) // +1 for recursion
            );
        }
    }

template<typename Tup, size_t I = 0>
static size_t getMinSize_tuple( const Tup & t )
    {
    if constexpr ( I == std::tuple_size_v<Tup> )
        return std::numeric_limits<size_t>::max();
    else
        return std::min( std::get<I>( t ).size(), getMinSize_tuple<Tup, I + 1>( t ) );
    }

template<typename Tup, size_t I = 0>
static size_t getMaxSize_tuple( const Tup & t )
    {
    if constexpr ( I == std::tuple_size_v<Tup> )
        return std::numeric_limits<size_t>::min();
    else
        return std::max( std::get<I>( t ).size(), getMaxSize_tuple<Tup, I + 1>( t ) );
    }

template<typename Tup, size_t I = 0>
auto luaF_LTMP_getCurrentTuple( Tup t, int i )
    {
    constexpr size_t numArgs = std::tuple_size_v<Tup>;

    if constexpr ( I == numArgs ) // If we have no more args to get
        return std::tuple();
    else  
        {
        auto vec = std::get<I>( t );
        return std::tuple_cat( 
            std::make_tuple( vec[ i % vec.size() ] ),
            luaF_LTMP_getCurrentTuple<Tup, I + 1>( t, i )
            );
        }
    }

template<class Functor, size_t numArgs>
static int luaF_LTMP_dispatched( lua_State* L )
    {
    // If this gets called from the main thread, thread will be nullptr and it will crash, just fyi
    auto & canceller = static_cast<FalterThread *>( Thread::getCurrentThread() )->getCanceller();
    auto && cancelledFunctor = std::bind_front( Functor(), std::ref( canceller ) );

    if constexpr( numArgs == 0 )
        static_assert( ! sizeof( Functor * ), "numArgs cannot be 0." );

    // Get args as a tuple of vectors
    auto vecTuple = luaF_wrapFunctor_checkArgs<Functor, numArgs>( L );

    const int maxArgLength = getMaxSize_tuple( vecTuple );
    const int minArgLength = getMinSize_tuple( vecTuple );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

    using R = liph::function_return_type<Functor>;

    if constexpr ( std::is_void_v<R> )
        {
        for( int i = 0; i < maxArgLength; ++i )
            {
            auto currentTuple = luaF_LTMP_getCurrentTuple( vecTuple, i );
            std::apply( cancelledFunctor, currentTuple );
            }
        return 0;
        }
    else
        {
        std::vector<R> outputs;
        outputs.reserve( maxArgLength );
        for( int i = 0; i < maxArgLength; ++i )
            {
            auto currentTuple = luaF_LTMP_getCurrentTuple( vecTuple, i );
            outputs.push_back( std::apply( cancelledFunctor, currentTuple ) );
            }
        luaF_LTMP_push( L, outputs );
        return 1;
        }
    }

// We want to call luaF_wrapFunctor<Functor, lua_gettop( L )>, but this requires passing a runtime value to a compile time parameter
// To get around this we make the call using a runtime dispatch
template<typename AlgoFunctor, size_t numNonDefaults, size_t I = liph::function_argument_count<AlgoFunctor> - 1> // -1 for canceller
static int luaF_LTMP( lua_State* L )
    {
    const size_t i = lua_gettop( L );

    if constexpr( I > numNonDefaults )
        {
        if( i == I ) return luaF_LTMP_dispatched<AlgoFunctor, I>( L );
        else return luaF_LTMP<AlgoFunctor, numNonDefaults, I - 1>( L );
        }
    else if constexpr ( I == numNonDefaults )
        {
        if( i == I ) return luaF_LTMP_dispatched<AlgoFunctor, I>( L );
        else luaL_error( L, "Too few arguments passed to flan function." );
        }
    // ! sizeof( T * ) is always false and type dependant. Using plain false does not compile.
    else static_assert( ! sizeof( AlgoFunctor * ), "Wrong number of non-defaulted arguments passed to a Lua function wrapper." );
    }



template<typename Functor, size_t numNonDefaults>
void luaF_register_helper( lua_State* L, const std::string & name )
    {
    lua_pushcclosure( L, luaF_LTMP<Functor, numNonDefaults>, 0 );  lua_setfield( L, -3, name.c_str() ); 
    lua_pushcclosure( L, luaF_LTMP<Functor, numNonDefaults>, 0 );  lua_setfield( L, -2, name.c_str() );
    }



// Old LTMP ===============================================================================================================

// template<class Functor, typename R, typename A>
// static int luaF_LTMP( lua_State* L )
//     {
//     // Get args
//     auto as = luaF_LTMP_check<A>( L, 1 );

//     const int maxArgLength = getMaxSize( as );
//     const int minArgLength = getMinSize( as );
//     if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

//     // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
//     FalterThread * thread = static_cast<FalterThread *>( Thread::getCurrentThread() );

//     std::vector<R> outputs;
//     outputs.reserve( maxArgLength );
//     for( int i = 0; i < maxArgLength; ++i )
//         outputs.push_back( Functor()( thread->getCanceller(),
//             as[ i % as.size() ]
//             ) );

//     luaF_LTMP_push( L, outputs );

//     return 1;
//     }

// template<class Functor, typename R, typename A, typename B>
// static int luaF_LTMP( lua_State* L )
//     {
//     if( lua_gettop( L ) < 2 ) return luaF_LTMP<Functor, R, A>( L );

//     // Get args
//     auto as = luaF_LTMP_check<A>( L, 1 );
//     auto bs = luaF_LTMP_check<B>( L, 2 );

//     const int maxArgLength = getMaxSize( as, bs );
//     const int minArgLength = getMinSize( as, bs );
//     if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

//     // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
//     FalterThread * thread = static_cast<FalterThread *>( Thread::getCurrentThread() );

//     std::vector<R> outputs;
//     outputs.reserve( maxArgLength );
//     for( int i = 0; i < maxArgLength; ++i )
//         outputs.push_back( Functor()( thread->getCanceller(),
//             as[ i % as.size() ],
//             bs[ i % bs.size() ]
//             ) );

//     luaF_LTMP_push( L, outputs );

//     return 1;
//     }

// template<class Functor, typename R, typename A, typename B, typename C>
// static int luaF_LTMP( lua_State* L )
//     {
//     if( lua_gettop( L ) < 3 ) return luaF_LTMP<Functor, R, A, B>( L );

//     // Get args
//     auto as = luaF_LTMP_check<A>( L, 1 );
//     auto bs = luaF_LTMP_check<B>( L, 2 );
//     auto cs = luaF_LTMP_check<C>( L, 3 );

//     const int maxArgLength = getMaxSize( as, bs, cs );
//     const int minArgLength = getMinSize( as, bs, cs );
//     if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

//     // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
//     FalterThread * thread = static_cast<FalterThread *>( Thread::getCurrentThread() );

//     std::vector<R> outputs;
//     outputs.reserve( maxArgLength );
//     for( int i = 0; i < maxArgLength; ++i )
//         outputs.push_back( Functor()( thread->getCanceller(),
//             as[ i % as.size() ],
//             bs[ i % bs.size() ],
//             cs[ i % cs.size() ]
//             ) );

//     luaF_LTMP_push( L, outputs );

//     return 1;
//     }

// template<class Functor, typename R, typename A, typename B, typename C, typename D>
// static int luaF_LTMP( lua_State* L )
//     {
//     if( lua_gettop( L ) < 4 ) return luaF_LTMP<Functor, R, A, B, C>( L );

//     // Get args
//     auto as = luaF_LTMP_check<A>( L, 1 );
//     auto bs = luaF_LTMP_check<B>( L, 2 );
//     auto cs = luaF_LTMP_check<C>( L, 3 );
//     auto ds = luaF_LTMP_check<D>( L, 4 );

//     const int maxArgLength = getMaxSize( as, bs, cs, ds );
//     const int minArgLength = getMinSize( as, bs, cs, ds );
//     if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

//     // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
//     FalterThread * thread = static_cast<FalterThread *>( Thread::getCurrentThread() );

//     std::vector<R> outputs;
//     outputs.reserve( maxArgLength );
//     for( int i = 0; i < maxArgLength; ++i )
//         outputs.push_back( Functor()( thread->getCanceller(),
//             as[ i % as.size() ],
//             bs[ i % bs.size() ],
//             cs[ i % cs.size() ],
//             ds[ i % ds.size() ]
//             ) );

//     luaF_LTMP_push( L, outputs );

//     return 1;
//     }

// template<class Functor, typename R, typename A, typename B, typename C, typename D, typename E>
// static int luaF_LTMP( lua_State* L )
//     {
//     if( lua_gettop( L ) < 5 ) return luaF_LTMP<Functor, R, A, B, C, D>( L );

//     // Get args
//     auto as = luaF_LTMP_check<A>( L, 1 );
//     auto bs = luaF_LTMP_check<B>( L, 2 );
//     auto cs = luaF_LTMP_check<C>( L, 3 );
//     auto ds = luaF_LTMP_check<D>( L, 4 );
//     auto es = luaF_LTMP_check<E>( L, 5 );

//     const int maxArgLength = getMaxSize( as, bs, cs, ds, es );
//     const int minArgLength = getMinSize( as, bs, cs, ds, es );
//     if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

//     // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
//     FalterThread * thread = static_cast<FalterThread *>( Thread::getCurrentThread() );

//     std::vector<R> outputs;
//     outputs.reserve( maxArgLength );
//     for( int i = 0; i < maxArgLength; ++i )
//         outputs.push_back( Functor()( thread->getCanceller(),
//             as[ i % as.size() ],
//             bs[ i % bs.size() ],
//             cs[ i % cs.size() ],
//             ds[ i % ds.size() ],
//             es[ i % es.size() ]
//             ) );

//     luaF_LTMP_push( L, outputs );

//     return 1;
//     }

// template<class Functor, typename R, typename A, typename B, typename C, typename D, typename E, typename F>
// static int luaF_LTMP( lua_State* L )
//     {
//     if( lua_gettop( L ) < 6 ) return luaF_LTMP<Functor, R, A, B, C, D, E>( L );

//     // Get args
//     auto as = luaF_LTMP_check<A>( L, 1 );
//     auto bs = luaF_LTMP_check<B>( L, 2 );
//     auto cs = luaF_LTMP_check<C>( L, 3 );
//     auto ds = luaF_LTMP_check<D>( L, 4 );
//     auto es = luaF_LTMP_check<E>( L, 5 );
//     auto fs = luaF_LTMP_check<F>( L, 6 );

//     const int maxArgLength = getMaxSize( as, bs, cs, ds, es, fs );
//     const int minArgLength = getMinSize( as, bs, cs, ds, es, fs );
//     if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

//     // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
//     FalterThread * thread = static_cast<FalterThread *>( Thread::getCurrentThread() );

//     std::vector<R> outputs;
//     outputs.reserve( maxArgLength );
//     for( int i = 0; i < maxArgLength; ++i )
//         outputs.push_back( Functor()( thread->getCanceller(),
//             as[ i % as.size() ],
//             bs[ i % bs.size() ],
//             cs[ i % cs.size() ],
//             ds[ i % ds.size() ],
//             es[ i % es.size() ],
//             fs[ i % fs.size() ]
//             ) );

//     luaF_LTMP_push( L, outputs );

//     return 1;
//     }

// template<class Functor, typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G>
// static int luaF_LTMP( lua_State* L )
//     {
//     if( lua_gettop( L ) < 6 ) return luaF_LTMP<Functor, R, A, B, C, D, E, F>( L );

//     // Get args
//     auto as = luaF_LTMP_check<A>( L, 1 );
//     auto bs = luaF_LTMP_check<B>( L, 2 );
//     auto cs = luaF_LTMP_check<C>( L, 3 );
//     auto ds = luaF_LTMP_check<D>( L, 4 );
//     auto es = luaF_LTMP_check<E>( L, 5 );
//     auto fs = luaF_LTMP_check<F>( L, 6 );
//     auto gs = luaF_LTMP_check<G>( L, 7 );

//     const int maxArgLength = getMaxSize( as, bs, cs, ds, es, fs, gs );
//     const int minArgLength = getMinSize( as, bs, cs, ds, es, fs, gs );
//     if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

//     // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
//     FalterThread * thread = static_cast<FalterThread *>( Thread::getCurrentThread() );

//     std::vector<R> outputs;
//     outputs.reserve( maxArgLength );
//     for( int i = 0; i < maxArgLength; ++i )
//         outputs.push_back( Functor()( thread->getCanceller(),
//             as[ i % as.size() ],
//             bs[ i % bs.size() ],
//             cs[ i % cs.size() ],
//             ds[ i % ds.size() ],
//             es[ i % es.size() ],
//             fs[ i % fs.size() ],
//             gs[ i % gs.size() ]
//             ) );

//     luaF_LTMP_push( L, outputs );

//     return 1;
//     }

// template<class Functor, typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H>
// static int luaF_LTMP( lua_State* L )
//     {
//     if( lua_gettop( L ) < 6 ) return luaF_LTMP<Functor, R, A, B, C, D, E, F, G>( L );

//     // Get args
//     auto as = luaF_LTMP_check<A>( L, 1 );
//     auto bs = luaF_LTMP_check<B>( L, 2 );
//     auto cs = luaF_LTMP_check<C>( L, 3 );
//     auto ds = luaF_LTMP_check<D>( L, 4 );
//     auto es = luaF_LTMP_check<E>( L, 5 );
//     auto fs = luaF_LTMP_check<F>( L, 6 );
//     auto gs = luaF_LTMP_check<G>( L, 7 );
//     auto hs = luaF_LTMP_check<H>( L, 8 );

//     const int maxArgLength = getMaxSize( as, bs, cs, ds, es, fs, gs, hs );
//     const int minArgLength = getMinSize( as, bs, cs, ds, es, fs, gs, hs );
//     if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

//     // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
//     FalterThread * thread = static_cast<FalterThread *>( Thread::getCurrentThread() );

//     std::vector<R> outputs;
//     outputs.reserve( maxArgLength );
//     for( int i = 0; i < maxArgLength; ++i )
//         outputs.push_back( Functor()( thread->getCanceller(),
//             as[ i % as.size() ],
//             bs[ i % bs.size() ],
//             cs[ i % cs.size() ],
//             ds[ i % ds.size() ],
//             es[ i % es.size() ],
//             fs[ i % fs.size() ],
//             gs[ i % gs.size() ],
//             hs[ i % hs.size() ]
//             ) );

//     luaF_LTMP_push( L, outputs );

//     return 1;
//     }

// template<typename ... Args>
// void luaF_register_helper( lua_State* L, const std::string & name )
//     {
//     lua_pushcclosure( L, luaF_LTMP<Args...>, 0 );  lua_setfield( L, -3, name.c_str() ); 
//     lua_pushcclosure( L, luaF_LTMP<Args...>, 0 );  lua_setfield( L, -2, name.c_str() );
//     }