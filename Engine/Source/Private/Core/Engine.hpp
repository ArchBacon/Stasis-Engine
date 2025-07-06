#pragma once

#include "Core/Types.hpp"

struct SDL_Window;

namespace Stasis
{
    class Engine
    {
        bool IsInitialized {false};
        bool StopRendering {false};
        bool IsRunning {true};
        unsigned int FrameNumber {0};
        int2 WindowExtent {1024, 576};

        SDL_Window* Window {nullptr};
        
    public:
        void Initialize();
        void Run();
        void Shutdown();
    };
}

extern Stasis::Engine Engine;
