#include "Function.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "Usertypes.h"
#include "Utility.h"
#include "LTMP.h"

using namespace flan;

// Interface ================================================================================================================
template<> bool luaF_isFunc<pFunc1x1>( lua_State * L, int i )
    {
    return lua_isnumber( L, i ) || lua_isfunction( L, i ) || luaF_isUsertype<pFunc1x1>( L, i );
    }
template<> bool luaF_isFunc<pFunc2x1>( lua_State * L, int i )
    {
    return lua_isnumber( L, i ) || lua_isfunction( L, i ) || luaF_isUsertype<pFunc2x1>( L, i );
    }
template<> bool luaF_isFunc<pFunc1x2>( lua_State * L, int i )
    {
    return luaF_is<vec2>( L, i ) || lua_isfunction( L, i ) || luaF_isUsertype<pFunc1x2>( L, i );
    }
template<> bool luaF_isFunc<pFunc2x2>( lua_State * L, int i )
    {
    return luaF_is<vec2>( L, i ) || lua_isfunction( L, i ) || luaF_isUsertype<pFunc2x2>( L, i );
    }

// This gets a number, lua function, or already defined flan Func type and returns a flan Function
template<typename T>
T luaF_checkFunc_base( lua_State * L, int i )
    {
    using F = typename T::element_type;
    using I = typename F::ArgType;
    using O = typename F::ReturnType;

    if( luaF_is<O>( L, i ) )
        return std::make_shared<F>( luaF_check<O>( L, i ) );
    else if( lua_isfunction( L, i ) ) // Lua function recieved, convert to userdata wrapped type
        {
        lua_pushvalue( L, i ); // Copy the function, ref will pop it. Check functions like this one shouldn't pop.
        const int ref = luaL_ref( L, LUA_REGISTRYINDEX );
        return std::make_shared<F>( [L, ref]( I in ) -> O
            {
            int num_args;

            lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
            if constexpr( std::is_same_v<I, flan::vec2> == true )
                {
                luaF_push( L, in.x() );
                luaF_push( L, in.y() );
                num_args = 2;
                }
            else
                {
                luaF_push( L, in );
                num_args = 1;
                }

            if constexpr( std::is_same_v<O, flan::vec2> == true )
                {
                lua_call( L, num_args, 2 );
                const vec2 out = { luaF_check<float>( L, -2 ), luaF_check<float>( L, -1 ) };
                lua_settop( L, 0 );
                return out;
                }
            else
                {
                lua_call( L, num_args, 1 );
                const auto out = luaF_check<O>( L, -1 );
                lua_settop( L, 0 );
                return out;
                }
            
            }, flan::ExecutionPolicy::Linear_Sequenced );
        }
    else if( luaF_isUsertype<T>( L, i ) )
        {
        return luaF_checkUsertype<T>( L, i );
        }
    else 
        {
        luaL_typerror( L, i, luaF_getUsertypeName<T>().c_str() );
        return T();
        }
    }

template<> pFunc1x1 luaF_checkFunc( lua_State * L, int i ) { return luaF_checkFunc_base<pFunc1x1>( L, i ); }
template<> pFunc2x1 luaF_checkFunc( lua_State * L, int i ) { return luaF_checkFunc_base<pFunc2x1>( L, i ); }
template<> pFunc1x2 luaF_checkFunc( lua_State * L, int i ) { return luaF_checkFunc_base<pFunc1x2>( L, i ); }
template<> pFunc2x2 luaF_checkFunc( lua_State * L, int i ) { return luaF_checkFunc_base<pFunc2x2>( L, i ); }


// Static functions ================================================================================================================

struct F_Func1x1_ADSR { pFunc1x1 operator()( 
    float a = 0, 
    float d = 1, 
    float s = 0, 
    float r = 1, 
    float sLvl = 0, 
    float aExp = 1, 
    float dExp = 1, 
    float rExp = 1 ) 
    { std::cout << "flan::ADSR";
    return std::make_shared<Func1x1>( flan::ADSR( a, d, s, r, sLvl, aExp, dExp, rExp ) ); } };



// Metamethods ================================================================================================================

// This is called when a flan::Function gets called on the Lua side.
template<typename T>
static int luaF_Func_call( lua_State * L )
    {
    try {
        using F = typename T::element_type;
        using I = typename F::ArgType;
        using O = typename F::ReturnType;

        const std::string name = luaF_getUsertypeName<T>();

        T f = luaF_check<T>( L, 1 );

        I in;

        // Check for number of inputs
        if constexpr( std::is_convertible_v<I, float> == true )
            {
            if( lua_gettop( L ) != 2 ) 
                throw std::runtime_error( name + " recieved the wrong number of arguments (expected 1): " + std::to_string( lua_gettop( L ) - 1 ) );
            in = luaF_check<I>( L, 2 );
            }
        else if constexpr( std::is_same_v<I, flan::vec2> == true )
            {
            if( lua_gettop( L ) != 3 ) 
                throw std::runtime_error( name + " recieved the wrong number of arguments (expected 2): " + std::to_string( lua_gettop( L ) - 1 ) );
            in = flan::vec2{ luaF_check<float>( L, 2 ), luaF_check<float>( L, 3 ) };
            }
        else throw std::runtime_error( L, "Hold on there buckaroo, how did you get a hold of this sort of function?" );

        // Clear function and args
        lua_settop( L, 0 ); 

        // Check for number of outputs
        if constexpr( std::is_convertible_v<O, float> == true )
            {
            const float out = (*f)( in );
            luaF_push( L, out );
            return 1;
            }
        else if constexpr( std::is_same_v<O, flan::vec2> == true )
            {
            const vec2 out = (*f)( in );
            luaF_push( L, out.x() );
            luaF_push( L, out.y() );
            return 2;
            }
        else throw std::runtime_error( L, "Hold on there buckaroo, how did you get a hold of this sort of function?" );
        }
    catch( std::exception & e ) { lua_pushstring( L, e.what() ); }
    lua_error( L );
    return 0;
    }


// Methods ================================================================================================================

template<typename T>
struct F_Func_constant_ctor 
    {
    using F = typename T::element_type;
    using O = typename F::ReturnType;
    T operator()( O out ){ return std::make_shared<F>( out ); } 
    };

template<typename T>
static int luaF_Func_ctor_selector( lua_State * L )
    {
    try
        {
        using O = typename T::element_type::ReturnType;

        if( lua_gettop( L ) < 2 )
            //return luaF_Usertype_new<T>( L );
            throw std::runtime_error( "Too few arguments to flan::Function constructor." );
        else if( luaF_is<O>( L, 2 ) || luaF_isArrayOfType<O>( L, 2 ) )
            return luaF_LTMP<F_Func_constant_ctor<T>, 1>( L );
        else if( lua_isfunction( L, 2 ) )
            {
            // Wrap the Lua function in a FuncAxB userdata type and push is back to the stack
            luaF_push( L, luaF_checkFunc_base<T>( L, 2 ) );
            return 1;
            }
        else throw std::runtime_error( "A flan::Function couldn't be constructed from the given arguments." );
        }
    catch( std::exception & e ) { lua_pushstring( L, e.what() ); }
    lua_error( L );
    return 0;
    }

struct F_Func1x1_save_to_bmp { void operator()( pFunc1x1 a, 
    const std::string & b, 
    Rect c = { -5, -5, 5, 5 }, 
    Interval d = Interval::R, 
    Pixel e = -1, 
    Pixel f = -1 ) 
    { std::cout << "flan::Function<I, float>::save_to_bmp";
    a->save_to_bmp( b, c, d, e, f ); } };

// struct F_Func1x1_periodize { pFunc1x1 operator()( pFunc1x1 f, 
//     Second period = 1.0f ) 
//     { std::cout << "flan::Function<float, O>::periodize";
//     return std::make_shared<Func1x1>( f->periodize( period ) ); } };



// ====================================================================================================================================================
// Registration 
// ====================================================================================================================================================

void luaF_register_function_types( lua_State* L )
    {
    // ====================================================================================================================================================
    // Func1x1 
    // ====================================================================================================================================================
    // Add static methods and ctor to the global Func1x1 table
    lua_newtable( L );
        lua_newtable( L );
            lua_pushcfunction( L, luaF_Func_ctor_selector<pFunc1x1> ); lua_setfield( L, -2, "__call" );
        lua_setmetatable( L, -2 );
        lua_pushcclosure( L, luaF_LTMP<F_Func1x1_ADSR, 5>, 0 ); lua_setfield( L, -2, "ADSR" );
    lua_setglobal( L, luaF_getUsertypeName<pFunc1x1>().c_str() );
    
    // Register the Func1x1 type which is constructer 
    luaL_newmetatable( L, luaF_getUsertypeName<pFunc1x1>().c_str() );
        lua_pushcfunction( L, luaF_Func_call<pFunc1x1> ); lua_setfield( L, -2, "__call" ); 
        lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );
        lua_register( L, luaF_getUsertypeName<Func1x1Vec>().c_str(), luaF_Usertype_vec_new<pFunc1x1> );
	    luaL_newmetatable( L, luaF_getUsertypeName<Func1x1Vec>().c_str() );
            luaF_register_helper<F_Func1x1_save_to_bmp, 2>( L, "save_to_bmp" );
            //luaF_register_helper<F_Func1x1_periodize, 2>( L, "periodize" );
    lua_pop( L, 2 );
   

    // ====================================================================================================================================================
    // Func2x1 
    // ====================================================================================================================================================
    lua_newtable( L );
        lua_newtable( L );
            lua_pushcfunction( L, luaF_Func_ctor_selector<pFunc2x1> ); lua_setfield( L, -2, "__call" );
        lua_setmetatable( L, -2 );
    lua_setglobal( L, luaF_getUsertypeName<pFunc2x1>().c_str() );

    luaL_newmetatable( L, luaF_getUsertypeName<pFunc2x1>().c_str() );
        lua_pushcfunction( L, luaF_Func_call<pFunc2x1> ); lua_setfield( L, -2, "__call" ); 
        lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );
        lua_register( L, luaF_getUsertypeName<Func2x1Vec>().c_str(), luaF_Usertype_vec_new<pFunc2x1> );
	    luaL_newmetatable( L, luaF_getUsertypeName<Func2x1Vec>().c_str() );
    lua_pop( L, 2 );


    // ====================================================================================================================================================
    // Func1x2 
    // ====================================================================================================================================================
    lua_newtable( L );
        lua_newtable( L );
            lua_pushcfunction( L, luaF_Func_ctor_selector<pFunc1x2> ); lua_setfield( L, -2, "__call" );
        lua_setmetatable( L, -2 );
    lua_setglobal( L, luaF_getUsertypeName<pFunc1x2>().c_str() );

    luaL_newmetatable( L, luaF_getUsertypeName<pFunc1x2>().c_str() );
        lua_pushcfunction( L, luaF_Func_call<pFunc1x2> ); lua_setfield( L, -2, "__call" ); 
        lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );
        lua_register( L, luaF_getUsertypeName<Func1x2Vec>().c_str(), luaF_Usertype_vec_new<pFunc1x2> );
	    luaL_newmetatable( L, luaF_getUsertypeName<Func1x2Vec>().c_str() );
    lua_pop( L, 2 );


    // ====================================================================================================================================================
    // Func2x2
    // ====================================================================================================================================================
    lua_newtable( L );  
        lua_newtable( L );
            lua_pushcfunction( L, luaF_Func_ctor_selector<pFunc2x2> ); lua_setfield( L, -2, "__call" );
        lua_setmetatable( L, -2 );
    lua_setglobal( L, luaF_getUsertypeName<pFunc2x2>().c_str() );
    
    luaL_newmetatable( L, luaF_getUsertypeName<pFunc2x2>().c_str() );
        lua_pushcfunction( L, luaF_Func_call<pFunc2x2> ); lua_setfield( L, -2, "__call" ); 
        lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );
        lua_register( L, luaF_getUsertypeName<Func2x2Vec>().c_str(), luaF_Usertype_vec_new<pFunc2x2> );
	    luaL_newmetatable( L, luaF_getUsertypeName<Func2x2Vec>().c_str() );
    lua_pop( L, 2 );
    }