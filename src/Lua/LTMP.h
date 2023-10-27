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

#include "./Types.h"

#include "Utility.h"

std::atomic<bool> & getThreadCanceller( lua_State * L );

// Retrieve a vector of T from the lua stack. 
// If a single T is on the stack, create a single element vector of it.
// If a Lua array of T is on the stack, collect each T from it into a vector.
// If any other type is at position i, error out.
template<typename T> 
std::vector<T> luaF_LTMP_check( lua_State * L, int i )
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

// Push a vector of T onto the Lua stack.
// The vector should never be empty, if it is, error out.
// If the vector has one element just push that element.
// If the vector has multiple elements, push them all into a Lua array.
template<typename T> 
void luaF_LTMP_push( lua_State* L, const std::vector<T> & os )
    {
    if( os.size() == 0 ) 
        luaL_error( L, "luaF_LTMP_push was called with an empty output vector" );
    if( os.size() == 1 ) 
        luaF_push( L, os[0] );
    else
        luaF_pushArrayOfType( L, os );
    }

// This recursively gets all the arguments needed for a flan function call from the Lua stack as a tuple.
// The elements of the tuple are all vectors of the type needed for the current argument type
template<typename Functor, size_t numArgs, size_t I = 0> 
constexpr auto luaF_wrapFunctor_checkArgs( lua_State * L ) 
    {
    if constexpr( I == numArgs )
        return std::tuple();
    else  
        {
        // Get the type of the Ith argument of the Functor
        // You can't get a reference from lua, so remove ref and const from the type
        using ArgType = std::remove_const_t< std::remove_reference_t< liph::function_argument_n<I, Functor> > >;

        return std::tuple_cat( 
            std::make_tuple( luaF_LTMP_check<ArgType>( L, I + 1 ) ), // +1 for lua indexing
            luaF_wrapFunctor_checkArgs<Functor, numArgs, I + 1>( L ) // +1 for recursion
            );
        }
    }

// LTMP tuples contain only vectors. 
// This gets the size of the vector with the smallest size within an LTMP tuple.
template<typename Tup, size_t I = 0>
static size_t getMinSize_tuple( const Tup & t )
    {
    if constexpr( I == std::tuple_size_v<Tup> )
        return std::numeric_limits<size_t>::max(); // At end of tuple, return a value that does not disrupt the min( min( ... ) ) chain.
    else
        return std::min( std::get<I>( t ).size(), getMinSize_tuple<Tup, I + 1>( t ) );
    }

// LTMP tuples contain only vectors. 
// This gets the size of the vector with the largest size within an LTMP tuple.
template<typename Tup, size_t I = 0>
static size_t getMaxSize_tuple( const Tup & t )
    {
    if constexpr( I == std::tuple_size_v<Tup> )
        return std::numeric_limits<size_t>::min(); // At end of tuple, return a value that does not disrupt the max( max( ... ) ) chain.
    else
        return std::max( std::get<I>( t ).size(), getMaxSize_tuple<Tup, I + 1>( t ) );
    }

// The LTMP system will run the flan algorithm requested as many times as the largest vector passed to it.
// As the other arguments may be vectors with less elements, those vectors are used modulo thier size.
// Given an index i, this returns a tuple not of LTMP vectors, but of elements from those vectors, for the ith LTMP step.
template<typename Tup, size_t I = 0>
auto luaF_LTMP_getCurrentTuple( Tup t, int i )
    {
    constexpr size_t numArgs = std::tuple_size_v<Tup>;

    if constexpr( I == numArgs ) // If we have no more args to get
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

// When a flan algorithm is called, you may optionally pass a Lua array where a single argument would normally be.
// The algorithm will then be run for each one of those array elements.
// This function does the heavy lifting for the LTMP system, going through the arguments on the Lua stack, getting the current
// set of arguments, appling the Functor to them, and collecting the results to be pushed back onto the Lua stack.
template<class Functor, size_t numArgs>
static int luaF_LTMP_dispatched( lua_State* L )
    {
    auto & canceller = getThreadCanceller( L );

    if constexpr( numArgs == 0 )
        static_assert( ! sizeof( Functor * ), "numArgs cannot be 0." );

    // Get args as a tuple of vectors
    auto vecTuple = luaF_wrapFunctor_checkArgs<Functor, numArgs>( L );

    const int maxArgLength = getMaxSize_tuple( vecTuple );
    const int minArgLength = getMinSize_tuple( vecTuple );
    if( minArgLength == 0 ) luaL_error( L, "Flan method recieved an empty argument in LTMP" );

    using R = liph::function_return_type<Functor>;

    if constexpr( std::is_void_v<R> ) // Handle functions which return nothing
        {
        for( int i = 0; i < maxArgLength; ++i )
            {
            auto currentTuple = luaF_LTMP_getCurrentTuple( vecTuple, i );
            std::apply( Functor(), currentTuple );
            if( canceller ) 
                return luaL_error( L, "Couldn't complete script, thread was cancelled" );
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
            outputs.push_back( std::apply( Functor(), currentTuple ) );
            if( canceller ) 
                return luaL_error( L, "Couldn't complete script, thread was cancelled" );
            }
        luaF_LTMP_push( L, outputs );
        return 1;
        }
    }

// We want to call luaF_wrapFunctor_checkArgs<Functor, lua_gettop( L )>, but this requires passing a runtime value to a compile time parameter.
// To get around this we make the call using a runtime dispatch.
// We start with I set to the number of arguments for Functor and check if we got that many args from Lua.
// If not, as long as there are more arguments that have defaults, recurse the function at I-1 and check if that's how many arguments were passed from Lua.
// If we get to the point that there are no more defaultable args and the number of Lua args still isn't equal to the number we check for, error out.
template<typename Functor, size_t numNonDefaults, size_t I = liph::function_argument_count<Functor>>
static int luaF_LTMP( lua_State* L )
    {
    // Get the number of arguments on the Lua stack
    const size_t numLuaInputs = static_cast<size_t>( lua_gettop( L ) );

    if constexpr( I > numNonDefaults ) // If we still have arguments that could be defaulted
        {
        if( numLuaInputs == I ) return luaF_LTMP_dispatched<Functor, I>( L );
        else return luaF_LTMP<Functor, numNonDefaults, I - 1>( L ); // 
        }
    else if constexpr( I == numNonDefaults )
        {
        if( numLuaInputs == I ) return luaF_LTMP_dispatched<Functor, I>( L );
        else return luaL_error( L, "Too few arguments passed to flan function." );
        }
    // ! sizeof( T * ) is always false and type dependant. Using plain false does not compile.
    else static_assert( ! sizeof( Functor * ), "Wrong number of non-defaulted arguments passed to a Lua function wrapper." );
    }

// Helper for pushing the correct LTMP method onto both the Lua type and the Lua array type.
template<typename Functor, size_t numNonDefaults>
void luaF_register_helper( lua_State* L, const std::string & name )
    {
    lua_pushcclosure( L, luaF_LTMP<Functor, numNonDefaults>, 0 );  lua_setfield( L, -3, name.c_str() ); 
    lua_pushcclosure( L, luaF_LTMP<Functor, numNonDefaults>, 0 );  lua_setfield( L, -2, name.c_str() );
    }
