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

// Input/Output type handlers
template<typename T> struct FuncTyper { typedef void In; typedef void Out; static const int numInputs = 0; static const int numOutputs = 0; };
template<> struct FuncTyper<Func1x1> { typedef float In; typedef float Out; static const int numInputs = 1; static const int numOutputs = 1; };
template<> struct FuncTyper<Func2x1> { typedef vec2 In;  typedef float Out; static const int numInputs = 2; static const int numOutputs = 1; };
template<> struct FuncTyper<Func2x2> { typedef vec2 In;  typedef vec2 Out;  static const int numInputs = 2; static const int numOutputs = 2; };

// These handle the fact that lua functions being cast to some flan Funcs take and return 1 or 2 args
template<typename T> T luaF_F_check( lua_State * L, int i ) {}
template<> float luaF_F_check( lua_State * L, int i ) { return luaL_checknumber( L, i ); }
template<> vec2 luaF_F_check( lua_State * L, int i ) 
    { 
    if( i > 0 )
        return vec2( luaL_checknumber( L, i ), luaL_checknumber( L, i + 1 ) ); 
    else
        return vec2( luaL_checknumber( L, i - 1 ), luaL_checknumber( L, i ) ); 
    }

template<typename T> void luaF_F_push( lua_State * L, T ) {}
template<> void luaF_F_push( lua_State * L, float x ) { lua_pushnumber( L, x ); }
template<> void luaF_F_push( lua_State * L, vec2 z ) { lua_pushnumber( L, z.x() ); lua_pushnumber( L, z.y() ); }



// Interface ================================================================================================================
template<> bool luaF_isFunc<Func1x1>( lua_State * L, int i )
    {
    return lua_isnumber( L, i ) || lua_isfunction( L, i ) || luaF_isUsertype<Func1x1>( L, i );
    }
template<> bool luaF_isFunc<Func2x1>( lua_State * L, int i )
    {
    return luaF_is<vec2>( L, i ) || lua_isfunction( L, i ) || luaF_isUsertype<Func2x1>( L, i );
    }
template<> bool luaF_isFunc<Func2x2>( lua_State * L, int i )
    {
    return luaF_is<vec2>( L, i ) || lua_isfunction( L, i ) || luaF_isUsertype<Func2x2>( L, i );
    }

// This gets a number, lua function, or already defined flan Func type and returns a flan Func
template<typename T>
T luaF_checkFunc_base( lua_State * L, int i )
    {
    using O = FuncTyper<T>::Out;

    const std::string name = luaF_getUsertypeName<T>();

    if( luaF_is<O>( L, i ) )
        return luaF_check<O>( L, i );
    else if( lua_isfunction( L, i ) ) // Lua function recieved, convert to userdata wrapped type
        {
        lua_pushvalue( L, i ); // Copy the function, ref will pop it. Check functions don't pop.
        const int ref = luaL_ref( L, LUA_REGISTRYINDEX );
        return [L, ref]( FuncTyper<T>::In in ) -> FuncTyper<T>::Out
            {
            lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
            luaF_F_push( L, in );
            lua_call( L, FuncTyper<T>::numInputs, FuncTyper<T>::numOutputs );
            const auto out = luaF_F_check<FuncTyper<T>::Out>( L, -1 );
            lua_pop( L, 1 );
            return out;
            };
        }
    else if( luaF_isUsertype<T>( L, i ) )
        {
        return luaF_checkUsertype<T>( L, i );
        }
    else 
        {
        luaL_typerror( L, i, name.c_str() );
        return T();
        }
    }

template<> Func1x1 luaF_checkFunc( lua_State * L, int i ) { return luaF_checkFunc_base<Func1x1>( L, i ); }
template<> Func2x1 luaF_checkFunc( lua_State * L, int i ) // This specialization handles 1x1 being convertable to 2x1
    {
    if( luaF_isUsertype<Func1x1>( L, i ) )
        return luaF_checkUsertype<Func1x1>( L, i );
    else
        return luaF_checkFunc_base<Func2x1>( L, i );
    }
template<> Func2x2 luaF_checkFunc( lua_State * L, int i ) { return luaF_checkFunc_base<Func2x2>( L, i ); }


// Static functions ================================================================================================================

struct F_Func1x1_ADSR { Func1x1 operator()( std::atomic<bool> &, float a = 0, 
    float d = 1, float s = 0, float r = 1, float sLvl = 0, float aExp = 1, float dExp = 1, float rExp = 1 ) 
    { return Func1x1::ADSR( a, d, s, r, sLvl, aExp, dExp, rExp ); } };

template<typename T>
struct F_Func_uniformDist { T operator()( std::atomic<bool> &, T a, T b = 0 )
    { return T::uniformDistribution( a, b ); } };

template<typename T>
struct F_Func_normalDist { T operator()( std::atomic<bool> &, T a, T b = 0 )
    { return T::normalDistribution( a, b ); } };



// Metamethods ================================================================================================================

// This is called when a flan Func gets called on the Lua side.
template<typename T>
static int luaF_Func_call( lua_State * L )
    {
    const std::string name = luaF_getUsertypeName<T>();
    const int numInputs = FuncTyper<T>::numInputs;
    const int numOutputs = FuncTyper<T>::numOutputs;

    if( lua_gettop( L ) != 1 + numInputs ) 
        return luaL_error( L, ( name + " recieved the wrong number of arguments: " + std::to_string( lua_gettop( L ) - 1 ) ).c_str() );

    // Get function
    T f = luaF_check<T>( L, 1 );

    // Get args
    typename FuncTyper<T>::In in = luaF_F_check<typename FuncTyper<T>::In>( L, -1 );

    // Clear function and args
    lua_settop( L, 0 ); 

    // Call function and push to stack
    luaF_F_push( L, f( in ) );

    if( lua_gettop( L ) != numOutputs ) 
        return luaL_error( L, ( name + " returned the wrong number of values: " + std::to_string( lua_gettop( L ) ) ).c_str() );

    return numOutputs;
    }

template<typename T> static int luaF_Func_add( lua_State * L ) { luaF_push<T>( L, luaF_check<T>( L, 1 ) + luaF_check<T>( L, 2 ) ); return 1; }
template<typename T> static int luaF_Func_sub( lua_State * L ) { luaF_push<T>( L, luaF_check<T>( L, 1 ) - luaF_check<T>( L, 2 ) ); return 1; }
template<typename T> static int luaF_Func_mul( lua_State * L ) { luaF_push<T>( L, luaF_check<T>( L, 1 ) * luaF_check<T>( L, 2 ) ); return 1; }
template<typename T> static int luaF_Func_div( lua_State * L ) { luaF_push<T>( L, luaF_check<T>( L, 1 ) / luaF_check<T>( L, 2 ) ); return 1; }
template<typename T> static int luaF_Func_mod( lua_State * L ) { luaF_push<T>( L, luaF_check<T>( L, 1 ) % luaF_check<T>( L, 2 ) ); return 1; }
template<typename T> static int luaF_Func_unm( lua_State * L ) { luaF_push<T>( L, -luaF_check<T>( L, 2 ) ); return 1; }


// Methods ================================================================================================================

template<typename T>
struct F_Func_O_ctor { T operator()( std::atomic<bool> &, FuncTyper<T>::Out out )
    { return T( out ); } };
template<typename T>
static int luaF_Func_ctor_selector( lua_State * L )
    {
    using O = FuncTyper<T>::Out;

    if( lua_gettop( L ) < 2 ) // Default ctor
        return luaF_Usertype_new<T>( L );
    else if( luaF_is<O>( L, 2 ) || luaF_isArrayOfType<O>( L, 2 ) ) // O ctor
            return luaF_LTMP<F_Func_O_ctor<T>, 1>( L );
    else if( lua_isfunction( L, 2 ) )
        return luaL_error( L, "I didn't think anyone would ever do this so I didn't deal with it, email loganmcbroom@gmail.com and I'll fix." );
    else return luaL_error( L, "Function couldn't be constructed from the given arguments." );
    }

struct F_Func1x1_saveAsBMP { Func1x1 operator()( std::atomic<bool> & z, Func1x1 a, const std::string & b = "Func1x1.bmp", 
    Rect c = { -5, -5, 5, 5 }, Interval d = Interval::R, Pixel e = -1, Pixel f = -1 ) 
    { return a.saveAsBMP( b, c, d, e, f, z ); } };

struct F_Func1x1_periodize { Func1x1 operator()( std::atomic<bool> &, Func1x1 f, float p = 1.0f ) 
    { return f.periodize( p ); } };


// Registration ================================================================================================================
template<typename T>
static void luaF_register_function_type( lua_State * L )
    {
    // Create the Func metatable
	luaL_newmetatable( L, luaF_getUsertypeName<T>().c_str() );
        lua_pushcfunction( L, luaF_Func_call<T> ); lua_setfield( L, -2, "__call" );
        lua_pushcfunction( L, luaF_Func_add <T> ); lua_setfield( L, -2, "__add"  );
        lua_pushcfunction( L, luaF_Func_sub <T> ); lua_setfield( L, -2, "__sub"  );
        lua_pushcfunction( L, luaF_Func_mul <T> ); lua_setfield( L, -2, "__mul"  );
        lua_pushcfunction( L, luaF_Func_div <T> ); lua_setfield( L, -2, "__div"  );
        lua_pushcfunction( L, luaF_Func_mod <T> ); lua_setfield( L, -2, "__mod"  );
        lua_pushcfunction( L, luaF_Func_unm <T> ); lua_setfield( L, -2, "__unm"  );
    }

void luaF_register_function_types( lua_State* L )
    {
    // Func1x1 ====================================================================================================================================================
    // Add static methods and ctor to the global Func1x1 table
    lua_newtable( L );
        lua_newtable( L );
            lua_pushcfunction( L, luaF_Func_ctor_selector<Func1x1> ); lua_setfield( L, -2, "__call" );
        lua_setmetatable( L, -2 );
        lua_pushcclosure( L, luaF_LTMP<F_Func1x1_ADSR, 5>, 0 ); lua_setfield( L, -2, "ADSR" );
        lua_pushcclosure( L, luaF_LTMP<F_Func_uniformDist<Func1x1>, 2>, 0 ); lua_setfield( L, -2, "uniformDistribution" );
        lua_pushcclosure( L, luaF_LTMP<F_Func_normalDist<Func1x1>, 2>, 0 ); lua_setfield( L, -2, "normalDistribution" );
    lua_setglobal( L, luaF_getUsertypeName<Func1x1>().c_str() );
    
    // Register the Func1x1 type which is constructer 
    luaF_register_function_type<Func1x1>( L ); 
        lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );
        lua_register( L, luaF_getUsertypeName<Func1x1Vec>().c_str(), luaF_Usertype_vec_new<Func1x1> );
	    luaL_newmetatable( L, luaF_getUsertypeName<Func1x1Vec>().c_str() );
            luaF_register_helper<F_Func1x1_saveAsBMP, 2>( L, "saveAsBMP" );
            luaF_register_helper<F_Func1x1_periodize, 2>( L, "periodize" );
    lua_pop( L, 2 );
   

    // Func2x1 ====================================================================================================================================================
    lua_newtable( L );
        lua_newtable( L );
            lua_pushcfunction( L, luaF_Func_ctor_selector<Func2x1> ); lua_setfield( L, -2, "__call" );
        lua_setmetatable( L, -2 );
        lua_pushcclosure( L, luaF_LTMP<F_Func_uniformDist<Func2x1>, 2>, 0 ); lua_setfield( L, -2, "uniformDistribution" );
        lua_pushcclosure( L, luaF_LTMP<F_Func_normalDist<Func2x1>, 2>, 0 ); lua_setfield( L, -2, "normalDistribution" );
    lua_setglobal( L, luaF_getUsertypeName<Func2x1>().c_str() );

    luaF_register_function_type<Func2x1>( L );
        lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );
        lua_register( L, luaF_getUsertypeName<Func2x1Vec>().c_str(), luaF_Usertype_vec_new<Func2x1> );
	    luaL_newmetatable( L, luaF_getUsertypeName<Func2x1Vec>().c_str() );
    lua_pop( L, 2 );


    // Func2x2 ====================================================================================================================================================
    lua_newtable( L );  
        lua_newtable( L );
            lua_pushcfunction( L, luaF_Func_ctor_selector<Func2x2> ); lua_setfield( L, -2, "__call" );
        lua_setmetatable( L, -2 );
    lua_setglobal( L, luaF_getUsertypeName<Func2x2>().c_str() );
    
    luaF_register_function_type<Func2x2>( L );
        lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );
        lua_register( L, luaF_getUsertypeName<Func2x2Vec>().c_str(), luaF_Usertype_vec_new<Func2x2> );
	    luaL_newmetatable( L, luaF_getUsertypeName<Func2x2Vec>().c_str() );
    lua_pop( L, 2 );
    }