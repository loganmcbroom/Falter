#include "Usertypes.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "Audio.h"
#include "PVOC.h"
#include "Function.h"

template<> std::string luaF_getUsertypeName<flan::Audio>()     { return "Audio";       }
template<> std::string luaF_getUsertypeName<AudioMod>()        { return "AudioMod";    }
template<> std::string luaF_getUsertypeName<AudioVec>()        { return "AudioVec";    }
template<> std::string luaF_getUsertypeName<flan::PVOC>()      { return "PVOC";        }
template<> std::string luaF_getUsertypeName<PVOCVec>()         { return "PVOCVec";     }
template<> std::string luaF_getUsertypeName<flan::Func1x1>()   { return "Func1x1";     }
template<> std::string luaF_getUsertypeName<flan::Func2x1>()   { return "Func2x1";     }
template<> std::string luaF_getUsertypeName<flan::Func2x2>()   { return "Func2x2";     }


// Register =======================================================================================================================

void luaF_register_Usertypes( lua_State * L )
    {
    luaF_register_Audio( L ); // Register Audio class into the Lua context
	luaF_register_PVOC( L ); // Register PVOC class into the Lua context
	luaF_register_function_types( L ); // Register all function types into the Lua context
    }

    

