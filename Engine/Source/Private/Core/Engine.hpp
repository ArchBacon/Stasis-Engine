#pragma once

#include "Core/Logger.hpp"

DECLARE_LOG_CATEGORY(LogEngine)

namespace Stasis
{
    class Engine
    {
        bool IsRunning {true};
        
    public:
        void Initialize();
        void Run();
        void Shutdown();
    };
}

extern Stasis::Engine Engine;
