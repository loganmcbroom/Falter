#pragma once

#include <vector>
#include <functional>

#include "./Types.h"


// LuaF_check
template<typename T> std::vector<T> luaF_check( lua_State* L, int i );
template<> std::vector<int      > luaF_check( lua_State* L, int i );
template<> std::vector<uint32_t > luaF_check( lua_State* L, int i );
template<> std::vector<float    > luaF_check( lua_State* L, int i );
template<> std::vector<s_Audio  > luaF_check( lua_State* L, int i );
//template<> std::vector<s_PVOC   > luaF_check( lua_State* L, int i );
template<> std::vector<s_Func1x1> luaF_check( lua_State* L, int i );


// LuaF_push
template<typename R>
void luaF_push( lua_State* L, const std::vector<R>& os );
template<> void luaF_push( lua_State* L, const std::vector<int>& os );
template<> void luaF_push( lua_State* L, const std::vector<uint32_t>& os );
template<> void luaF_push( lua_State* L, const std::vector<float>& os );
template<> void luaF_push( lua_State* L, const std::vector<s_Audio>& os );


// getMaxSize - Calls size() on each input and returns the largest result
template<typename T>
static int getMaxSize(const T & t)
    {
    return t.size();
    }
template<typename T, typename ... Rest>
static int getMaxSize(const T & t, const Rest & ... rest)
    {
    return std::max( (size_t) t.size(), (size_t) getMaxSize( rest... ) );
    }

// getMaxSize - Calls size() on each input and returns the largest result
template<typename T>
static int getMinSize(const T & t)
    {
    return t.size();
    }
template<typename T, typename ... Rest>
static int getMinSize(const T & t, const Rest & ... rest)
    {
    return std::min( (size_t) t.size(), (size_t) getMinSize( rest... ) );
    }

bool luaF_isUserType( lua_State *, int i, const char * );

    
// LTMP ===============================================================================================================================

template<class Functor, typename R, typename A>
static int luaF_LTMP( lua_State* L )
    {
    // Get args
    auto as = luaF_check<A>( L, 1 );

    const int maxArgLength = getMaxSize( as );
    const int minArgLength = getMinSize( as );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an unusable argument in LTMP" );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( 
            as[ i % as.size() ]
            ) );

    luaF_push<R>( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 2 ) return luaF_LTMP<Functor, R, A>( L );

    // Get args
    auto as = luaF_check<A>( L, 1 );
    auto bs = luaF_check<B>( L, 2 );

    const int maxArgLength = getMaxSize( as, bs );
    const int minArgLength = getMinSize( as, bs );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an unusable argument in LTMP" );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( 
            as[ i % as.size() ],
            bs[ i % bs.size() ]
            ) );

    luaF_push<R>( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B, typename C>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 3 ) return luaF_LTMP<Functor, R, A, B>( L );

    // Get args
    auto as = luaF_check<A>( L, 1 );
    auto bs = luaF_check<B>( L, 2 );
    auto cs = luaF_check<C>( L, 3 );

    const int maxArgLength = getMaxSize( as, bs, cs );
    const int minArgLength = getMinSize( as, bs, cs );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an unusable argument in LTMP" );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( 
            as[ i % as.size() ],
            bs[ i % bs.size() ],
            cs[ i % cs.size() ]
            ) );

    luaF_push<R>( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B, typename C, typename D>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 4 ) return luaF_LTMP<Functor, R, A, B, C>( L );

    // Get args
    auto as = luaF_check<A>( L, 1 );
    auto bs = luaF_check<B>( L, 2 );
    auto cs = luaF_check<C>( L, 3 );
    auto ds = luaF_check<D>( L, 4 );

    const int maxArgLength = getMaxSize( as, bs, cs, ds );
    const int minArgLength = getMinSize( as, bs, cs, ds );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an unusable argument in LTMP" );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( 
            as[ i % as.size() ],
            bs[ i % bs.size() ],
            cs[ i % cs.size() ],
            ds[ i % ds.size() ]
            ) );

    luaF_push<R>( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B, typename C, typename D, typename E>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 5 ) return luaF_LTMP<Functor, R, A, B, C, D>( L );

    // Get args
    auto as = luaF_check<A>( L, 1 );
    auto bs = luaF_check<B>( L, 2 );
    auto cs = luaF_check<C>( L, 3 );
    auto ds = luaF_check<D>( L, 4 );
    auto es = luaF_check<E>( L, 5 );

    const int maxArgLength = getMaxSize( as, bs, cs, ds, es );
    const int minArgLength = getMinSize( as, bs, cs, ds, es );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an unusable argument in LTMP" );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( 
            as[ i % as.size() ],
            bs[ i % bs.size() ],
            cs[ i % cs.size() ],
            ds[ i % ds.size() ],
            es[ i % es.size() ]
            ) );

    luaF_push<R>( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B, typename C, typename D, typename E, typename F>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 6 ) return luaF_LTMP<Functor, R, A, B, C, D, E>( L );

    // Get args
    auto as = luaF_check<A>( L, 1 );
    auto bs = luaF_check<B>( L, 2 );
    auto cs = luaF_check<C>( L, 3 );
    auto ds = luaF_check<D>( L, 4 );
    auto es = luaF_check<E>( L, 5 );
    auto fs = luaF_check<F>( L, 6 );

    const int maxArgLength = getMaxSize( as, bs, cs, ds, es, fs );
    const int minArgLength = getMinSize( as, bs, cs, ds, es, fs );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an unusable argument in LTMP" );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( 
            as[ i % as.size() ],
            bs[ i % bs.size() ],
            cs[ i % cs.size() ],
            ds[ i % ds.size() ],
            es[ i % es.size() ],
            fs[ i % fs.size() ]
            ) );

    luaF_push<R>( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 6 ) return luaF_LTMP<Functor, R, A, B, C, D, E, F>( L );

    // Get args
    auto as = luaF_check<A>( L, 1 );
    auto bs = luaF_check<B>( L, 2 );
    auto cs = luaF_check<C>( L, 3 );
    auto ds = luaF_check<D>( L, 4 );
    auto es = luaF_check<E>( L, 5 );
    auto fs = luaF_check<F>( L, 6 );
    auto gs = luaF_check<G>( L, 7 );

    const int maxArgLength = getMaxSize( as, bs, cs, ds, es, fs, gs );
    const int minArgLength = getMinSize( as, bs, cs, ds, es, fs, gs );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an unusable argument in LTMP" );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( 
            as[ i % as.size() ],
            bs[ i % bs.size() ],
            cs[ i % cs.size() ],
            ds[ i % ds.size() ],
            es[ i % es.size() ],
            fs[ i % fs.size() ],
            gs[ i % gs.size() ]
            ) );

    luaF_push<R>( L, outputs );

    return 1;
    }

template<class Functor, typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H>
static int luaF_LTMP( lua_State* L )
    {
    if( lua_gettop( L ) < 6 ) return luaF_LTMP<Functor, R, A, B, C, D, E, F, G>( L );

    // Get args
    auto as = luaF_check<A>( L, 1 );
    auto bs = luaF_check<B>( L, 2 );
    auto cs = luaF_check<C>( L, 3 );
    auto ds = luaF_check<D>( L, 4 );
    auto es = luaF_check<E>( L, 5 );
    auto fs = luaF_check<F>( L, 6 );
    auto gs = luaF_check<G>( L, 7 );
    auto hs = luaF_check<H>( L, 8 );

    const int maxArgLength = getMaxSize( as, bs, cs, ds, es, fs, gs, hs );
    const int minArgLength = getMinSize( as, bs, cs, ds, es, fs, gs, hs );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an unusable argument in LTMP" );

    std::vector<R> outputs;
    for( int i = 0; i < maxArgLength; ++i )
        outputs.push_back( Functor()( 
            as[ i % as.size() ],
            bs[ i % bs.size() ],
            cs[ i % cs.size() ],
            ds[ i % ds.size() ],
            es[ i % es.size() ],
            fs[ i % fs.size() ],
            gs[ i % gs.size() ],
            hs[ i % hs.size() ]
            ) );

    luaF_push<R>( L, outputs );

    return 1;
    }