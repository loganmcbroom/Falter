#pragma once 

#include <vector>
#include <memory>

struct lua_State;

namespace flan 
    { 
    class Audio; 
    class PVOC;
    struct Func1x1; 
    struct Func2x1; 
    struct Func2x2; 
    }

using s_Audio = std::shared_ptr<flan::Audio>;
using AudioVec = std::vector<s_Audio>;

using s_PVOC = std::shared_ptr<flan::PVOC>;
using PVOCVec = std::vector<s_PVOC>;

using s_Func1x1 = std::shared_ptr<flan::Func1x1>;
using s_Func2x1 = std::shared_ptr<flan::Func2x1>;
using s_Func2x2 = std::shared_ptr<flan::Func2x2>;