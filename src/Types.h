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
    class Wavetable;
    struct MF;
    }

using AudioMod = std::function< flan::Audio ( const flan::Audio &, flan::Time ) >;
using PrismFunc = std::function<flan::MF ( int note, flan::Time, int harmonic, flan::Frequency contourFreq, const std::vector<flan::Magnitude> & harmonicMagnitudes )>;

using AudioVec      = std::vector<flan::Audio>;
using PVOCVec       = std::vector<flan::PVOC>;
using Func1x1Vec    = std::vector<flan::Func1x1>;
using Func2x1Vec    = std::vector<flan::Func2x1>;
using Func2x2Vec    = std::vector<flan::Func2x2>;
using WavetableVec  = std::vector<flan::Wavetable>;