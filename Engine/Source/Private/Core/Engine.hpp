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

        float deltaTime {0.0f};
        
    public:
        void Initialize();
        void Run();
        void Shutdown();

        [[nodiscard]] Window& Window() const { return *window; }
        
        [[nodiscard]] float DeltaTime() const { return deltaTime; }
    };
}

extern blackbox::Engine Engine;
