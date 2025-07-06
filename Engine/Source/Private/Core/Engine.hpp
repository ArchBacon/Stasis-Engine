#pragma once

#include "Core/Types.hpp"

struct SDL_Window;

namespace Stasis
{
    class Engine
    {
        bool isInitialized {false};
        bool stopRendering {false};
        bool isRunning {true};
        unsigned int frameNumber {0};
        int2 windowExtent {1024, 576};

        SDL_Window* window {nullptr};
        
    public:
        void Initialize();
        void Run();
        void Shutdown();
    };
}

extern Stasis::Engine gEngine;
