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
    class PrismFunc;

    template<typename T>
    class SoundMod;

    using AudioMod = SoundMod<flan::Audio>;
    using PVMod = SoundMod<flan::PV>;

    template<typename I, typename O>
    struct Function;

    template<typename T>
    struct vec2Base;
    using vec2 = vec2Base<float>;
    }

using pAudio                = std::shared_ptr<flan::Audio>;
using pPV                   = std::shared_ptr<flan::PV>;
using pWavetable            = std::shared_ptr<flan::Wavetable>;
using pGraph                = std::shared_ptr<flan::Graph>;

template<typename T>
using VecP = std::vector<std::shared_ptr<T>>;
using AudioVec      = VecP<flan::Audio>;
using PVVec         = VecP<flan::PV>;
using WavetableVec  = VecP<flan::Wavetable>;

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

template<typename T>
using pSoundMod             = std::shared_ptr<flan::SoundMod<T>>;
using pAudioMod             = std::shared_ptr<flan::AudioMod>;
using pPVMod                = std::shared_ptr<flan::PVMod>;

using pPrismFunc            = std::shared_ptr<flan::PrismFunc>;
using pGrainSource          = std::shared_ptr<flan::Function<flan::Second, flan::Audio>>;

template<typename T> using VecPredicate     = flan::Function<std::pair<std::shared_ptr<T>, int>, bool>;
template<typename T> using pVecPredicate    = std::shared_ptr<VecPredicate<T>>;
template<typename T> using VecGroupFunc     = flan::Function<std::pair<VecP<T>, int>, VecP<T>>;
template<typename T> using pVecGroupFunc    = std::shared_ptr<VecGroupFunc<T>>;

// Dummy type for fixing the dummy mistake c++ made making vector of bool not work correctly
struct Fool { 
    Fool( bool b_ ) : b( b_ ) {}
    bool b; 
};