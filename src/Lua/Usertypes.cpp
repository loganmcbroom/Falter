#include "Usertypes.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "GlobalFunctions.h"
#include "Audio.h"
#include "PV.h"
#include "Wavetable.h"
#include "Function.h"
#include "Interpolators.h"

template<> std::string luaF_getUsertypeName<pAudio>()               { return "Audio";            }
template<> std::string luaF_getUsertypeName<pPV>()                  { return "PV";               }
template<> std::string luaF_getUsertypeName<pFunc1x1>()             { return "Func1x1";          }
template<> std::string luaF_getUsertypeName<pFunc2x1>()             { return "Func2x1";          }
template<> std::string luaF_getUsertypeName<pFunc1x2>()             { return "Func1x2";          }
template<> std::string luaF_getUsertypeName<pFunc2x2>()             { return "Func2x2";          }
template<> std::string luaF_getUsertypeName<pWavetable>()           { return "Wavetable";        }
template<> std::string luaF_getUsertypeName<InterpolatorIndex>()    { return "Interp";           }

template<> std::string luaF_getUsertypeName<AudioVec>()             { return "AudioVec";         }
template<> std::string luaF_getUsertypeName<PVVec>()                { return "PVVec";            }
template<> std::string luaF_getUsertypeName<Func1x1Vec>()           { return "Func1x1Vec";       }
template<> std::string luaF_getUsertypeName<Func2x1Vec>()           { return "Func2x1Vec";       }
template<> std::string luaF_getUsertypeName<Func1x2Vec>()           { return "Func1x2Vec";       }
template<> std::string luaF_getUsertypeName<Func2x2Vec>()           { return "Func2x2Vec";       }
template<> std::string luaF_getUsertypeName<WavetableVec>()         { return "WavetableVec";     }

template<> std::string luaF_getUsertypeName<pSoundMod<flan::Audio>>()       { return "AudioMod";            }
template<> std::string luaF_getUsertypeName<pSoundMod<flan::PV>>()          { return "PVMod";               }
template<> std::string luaF_getUsertypeName<pPrismFunc>()                   { return "PrismFunc";           }
template<> std::string luaF_getUsertypeName<pGrainSource>()                 { return "GrainSource";         }
template<> std::string luaF_getUsertypeName<pVecPredicate<flan::Audio>>()   { return "AudioVecPredicate";   }
template<> std::string luaF_getUsertypeName<pVecGroupFunc<flan::Audio>>()   { return "AudioVecGroupFunc";   }
template<> std::string luaF_getUsertypeName<pVecPredicate<flan::PV>>()      { return "PVVecPredicate";      }
template<> std::string luaF_getUsertypeName<pVecGroupFunc<flan::PV>>()      { return "PVVecGroupFunc";      }


// Register =======================================================================================================================

void luaF_register_Usertypes( lua_State * L )
    {
    luaF_register_global_functions( L );
    luaF_register_Audio( L ); // Register Audio class into the Lua context
	luaF_register_PV( L ); // Register PV class into the Lua context
	luaF_register_Wavetable( L );
    luaF_register_function_types( L ); // Register all function types into the Lua context
    luaF_register_Interpolators( L );
    }

    

