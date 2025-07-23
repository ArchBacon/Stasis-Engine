#pragma once

#include <memory>

#include "Core/Types.hpp"

namespace blackbox::graphics
{
    class GlRenderer;
}

namespace blackbox
{
    class Window;
    
    class Engine
    {
        std::unique_ptr<Window> window {nullptr};
        std::unique_ptr<graphics::GlRenderer> renderer {nullptr};
        
        bool stopRendering {false};
        bool isRunning {true};
        uint32_t frameNumber {0};

        float deltaTime {0.0f};
        
    public:
        void Initialize();
        void Run();
        void Shutdown();

        [[nodiscard]] Window& Window() const { return *window; }
        [[nodiscard]] graphics::GlRenderer& Renderer() const { return *renderer; }
        
        [[nodiscard]] float DeltaTime() const { return deltaTime; }
    };
}

extern blackbox::Engine Engine;
