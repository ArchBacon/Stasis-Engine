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
        int2 WindowExtent {720, 405};
        
        SDL_Window* Window {nullptr};
        
    public:
        void Initialize();
        void Run();
        void Draw();
        void Shutdown();
    };
}

extern Stasis::Engine Engine;
