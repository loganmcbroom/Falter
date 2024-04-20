#pragma once 

#include <vector>
#include <functional>
#include <memory>

#include <flan/defines.h>

struct lua_State;

namespace flan 
    {
    class Audio;
    class PV;
    class Graph;
    struct Interval;
    struct Rect;
    class Wavetable;
    struct MF;
    struct TF;
    class AudioMod;
    class PrismFunc;

    template<typename I, typename O>
    struct Function;

    template<typename T>
    struct vec2Base;
    using vec2 = vec2Base<float>;
    }

using pAudio            = std::shared_ptr<flan::Audio>;
using pPV               = std::shared_ptr<flan::PV>;
using pWavetable        = std::shared_ptr<flan::Wavetable>;
using pGraph            = std::shared_ptr<flan::Graph>;
using pAudioMod         = std::shared_ptr<flan::AudioMod>;
using pPrismFunc        = std::shared_ptr<flan::PrismFunc>;
using pGrainSource      = std::shared_ptr<flan::Function<flan::Second, flan::Audio>>;
using ModIfPredicate    = flan::Function<std::pair<int, pAudio>, bool>;
using pModIfPredicate   = std::shared_ptr<ModIfPredicate>;

using AudioVec      = std::vector<pAudio>;
using PVVec         = std::vector<pPV>;
using WavetableVec  = std::vector<pWavetable>;

using Func1x1 = flan::Function<float, float>;
using Func2x1 = flan::Function<flan::vec2, float>;
using Func1x2 = flan::Function<float, flan::vec2>;
using Func2x2 = flan::Function<flan::vec2, flan::vec2>;
using pFunc1x1 = std::shared_ptr<Func1x1>;
using pFunc2x1 = std::shared_ptr<Func2x1>;
using pFunc1x2 = std::shared_ptr<Func1x2>;
using pFunc2x2 = std::shared_ptr<Func2x2>;
using Func1x1Vec = std::vector<pFunc1x1>;
using Func2x1Vec = std::vector<pFunc2x1>;
using Func1x2Vec = std::vector<pFunc1x2>;
using Func2x2Vec = std::vector<pFunc2x2>;

// Dummy type for fixing the dummy mistake c++ made making vector of bool not work correctly
struct Fool { 
    Fool( bool b_ ) : b( b_ ) {}
    bool b; 
};