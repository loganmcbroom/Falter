#pragma once 

#include <vector>
#include <functional>

#include <flan/defines.h>

struct lua_State;

namespace flan 
    {
    class Audio;
    class PVOC;
    struct Func1x1; 
    struct Func2x1; 
    struct Func2x2; 
    class Graph;
    struct Interval;
    struct Rect;
    }

using AudioMod = std::function< flan::Audio ( const flan::Audio &, flan::Time ) >;
using AudioVec = std::vector<flan::Audio>;
using PVOCVec = std::vector<flan::PVOC>;