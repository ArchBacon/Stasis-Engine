#pragma once

#include <memory>

#include "Core/Types.hpp"

namespace blackbox
{
    class Window;
    
    class Engine
    {
        std::unique_ptr<Window> window {nullptr};
        
        bool stopRendering {false};
        bool isRunning {true};
        uint32_t frameNumber {0};
        
    public:
        void Initialize();
        void Run();
        void Shutdown();

        [[nodiscard]] Window& Window() const { return *window; }
    };
}

extern blackbox::Engine Engine;
