#pragma once
#include <chrono>

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
