#pragma once

#include <vector>
#include <functional>

#include <JuceHeader.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "./AltarThread.h"
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
        luaL_error( L, ( std::string( "Expected type: " ) + typeid( T ).name() ).c_str() );
        return {};
        }
    }
template<typename T> void luaF_LTMP_push( lua_State* L, const std::vector<T> & os )
    {
    if( os.size() == 0 ) 
        luaL_error( L, "luaF_push_base was called with an empty output vector" );
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

template<class Functor, typename R, typename A>
static int luaF_LTMP( lua_State* L )
    {
    // Get args
    auto as = luaF_LTMP_check<A>( L, 1 );

    const int maxArgLength = getMaxSize( as );
    const int minArgLength = getMinSize( as );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

    // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
    AltarThread * thread = static_cast<AltarThread *>( Thread::getCurrentThread() );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( thread->getCanceller(),
            as[ i % as.size() ]
            ) );

    luaF_LTMP_push( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 2 ) return luaF_LTMP<Functor, R, A>( L );

    // Get args
    auto as = luaF_LTMP_check<A>( L, 1 );
    auto bs = luaF_LTMP_check<B>( L, 2 );

    const int maxArgLength = getMaxSize( as, bs );
    const int minArgLength = getMinSize( as, bs );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

    // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
    AltarThread * thread = static_cast<AltarThread *>( Thread::getCurrentThread() );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( thread->getCanceller(),
            as[ i % as.size() ],
            bs[ i % bs.size() ]
            ) );

    luaF_LTMP_push( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B, typename C>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 3 ) return luaF_LTMP<Functor, R, A, B>( L );

    // Get args
    auto as = luaF_LTMP_check<A>( L, 1 );
    auto bs = luaF_LTMP_check<B>( L, 2 );
    auto cs = luaF_LTMP_check<C>( L, 3 );

    const int maxArgLength = getMaxSize( as, bs, cs );
    const int minArgLength = getMinSize( as, bs, cs );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

    // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
    AltarThread * thread = static_cast<AltarThread *>( Thread::getCurrentThread() );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( thread->getCanceller(),
            as[ i % as.size() ],
            bs[ i % bs.size() ],
            cs[ i % cs.size() ]
            ) );

    luaF_LTMP_push( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B, typename C, typename D>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 4 ) return luaF_LTMP<Functor, R, A, B, C>( L );

    // Get args
    auto as = luaF_LTMP_check<A>( L, 1 );
    auto bs = luaF_LTMP_check<B>( L, 2 );
    auto cs = luaF_LTMP_check<C>( L, 3 );
    auto ds = luaF_LTMP_check<D>( L, 4 );

    const int maxArgLength = getMaxSize( as, bs, cs, ds );
    const int minArgLength = getMinSize( as, bs, cs, ds );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

    // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
    AltarThread * thread = static_cast<AltarThread *>( Thread::getCurrentThread() );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( thread->getCanceller(),
            as[ i % as.size() ],
            bs[ i % bs.size() ],
            cs[ i % cs.size() ],
            ds[ i % ds.size() ]
            ) );

    luaF_LTMP_push( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B, typename C, typename D, typename E>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 5 ) return luaF_LTMP<Functor, R, A, B, C, D>( L );

    // Get args
    auto as = luaF_LTMP_check<A>( L, 1 );
    auto bs = luaF_LTMP_check<B>( L, 2 );
    auto cs = luaF_LTMP_check<C>( L, 3 );
    auto ds = luaF_LTMP_check<D>( L, 4 );
    auto es = luaF_LTMP_check<E>( L, 5 );

    const int maxArgLength = getMaxSize( as, bs, cs, ds, es );
    const int minArgLength = getMinSize( as, bs, cs, ds, es );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

    // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
    AltarThread * thread = static_cast<AltarThread *>( Thread::getCurrentThread() );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( thread->getCanceller(),
            as[ i % as.size() ],
            bs[ i % bs.size() ],
            cs[ i % cs.size() ],
            ds[ i % ds.size() ],
            es[ i % es.size() ]
            ) );

    luaF_LTMP_push( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B, typename C, typename D, typename E, typename F>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 6 ) return luaF_LTMP<Functor, R, A, B, C, D, E>( L );

    // Get args
    auto as = luaF_LTMP_check<A>( L, 1 );
    auto bs = luaF_LTMP_check<B>( L, 2 );
    auto cs = luaF_LTMP_check<C>( L, 3 );
    auto ds = luaF_LTMP_check<D>( L, 4 );
    auto es = luaF_LTMP_check<E>( L, 5 );
    auto fs = luaF_LTMP_check<F>( L, 6 );

    const int maxArgLength = getMaxSize( as, bs, cs, ds, es, fs );
    const int minArgLength = getMinSize( as, bs, cs, ds, es, fs );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

    // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
    AltarThread * thread = static_cast<AltarThread *>( Thread::getCurrentThread() );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( thread->getCanceller(),
            as[ i % as.size() ],
            bs[ i % bs.size() ],
            cs[ i % cs.size() ],
            ds[ i % ds.size() ],
            es[ i % es.size() ],
            fs[ i % fs.size() ]
            ) );

    luaF_LTMP_push( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 6 ) return luaF_LTMP<Functor, R, A, B, C, D, E, F>( L );

    // Get args
    auto as = luaF_LTMP_check<A>( L, 1 );
    auto bs = luaF_LTMP_check<B>( L, 2 );
    auto cs = luaF_LTMP_check<C>( L, 3 );
    auto ds = luaF_LTMP_check<D>( L, 4 );
    auto es = luaF_LTMP_check<E>( L, 5 );
    auto fs = luaF_LTMP_check<F>( L, 6 );
    auto gs = luaF_LTMP_check<G>( L, 7 );

    const int maxArgLength = getMaxSize( as, bs, cs, ds, es, fs, gs );
    const int minArgLength = getMinSize( as, bs, cs, ds, es, fs, gs );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

    // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
    AltarThread * thread = static_cast<AltarThread *>( Thread::getCurrentThread() );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( thread->getCanceller(),
            as[ i % as.size() ],
            bs[ i % bs.size() ],
            cs[ i % cs.size() ],
            ds[ i % ds.size() ],
            es[ i % es.size() ],
            fs[ i % fs.size() ],
            gs[ i % gs.size() ]
            ) );

    luaF_LTMP_push( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 6 ) return luaF_LTMP<Functor, R, A, B, C, D, E, F, G>( L );

    // Get args
    auto as = luaF_LTMP_check<A>( L, 1 );
    auto bs = luaF_LTMP_check<B>( L, 2 );
    auto cs = luaF_LTMP_check<C>( L, 3 );
    auto ds = luaF_LTMP_check<D>( L, 4 );
    auto es = luaF_LTMP_check<E>( L, 5 );
    auto fs = luaF_LTMP_check<F>( L, 6 );
    auto gs = luaF_LTMP_check<G>( L, 7 );
    auto hs = luaF_LTMP_check<H>( L, 8 );

    const int maxArgLength = getMaxSize( as, bs, cs, ds, es, fs, gs, hs );
    const int minArgLength = getMinSize( as, bs, cs, ds, es, fs, gs, hs );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

    // If this gets called from the main thread, thread will be nullptr and it will crash, juce fyi
    AltarThread * thread = static_cast<AltarThread *>( Thread::getCurrentThread() );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( thread->getCanceller(),
            as[ i % as.size() ],
            bs[ i % bs.size() ],
            cs[ i % cs.size() ],
            ds[ i % ds.size() ],
            es[ i % es.size() ],
            fs[ i % fs.size() ],
            gs[ i % gs.size() ],
            hs[ i % hs.size() ]
            ) );

    luaF_LTMP_push( L, outputs );

    return 1;
    }

template<typename ... Args>
void luaF_register_helper( lua_State* L, const std::string & name )
    {
    lua_pushcclosure( L, luaF_LTMP<Args...>, 0 );  lua_setfield( L, -3, name.c_str() ); 
    lua_pushcclosure( L, luaF_LTMP<Args...>, 0 );  lua_setfield( L, -2, name.c_str() );
    }