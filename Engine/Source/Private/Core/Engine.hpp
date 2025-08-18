#pragma once

#include <memory>
#include "Core/Types.hpp"

namespace blackbox
{
    class Container;

    class Engine
    {
        std::unique_ptr<blackbox::Container> container {nullptr};
        
        bool stopRendering {false};
        bool isRunning {true};
        uint32_t frameNumber {0};

        float deltaTime {0.0f};
        float uptime {0.0f};
        
    public:
        void Initialize();
        void Run();
        void Shutdown();

        [[nodiscard]] blackbox::Container& Container() const { return *container; }
        
        [[nodiscard]] float DeltaTime() const { return deltaTime; }
        // How long the engine has een running in seconds
        [[nodiscard]] float Uptime() const { return uptime; } 
        [[nodiscard]] uint32_t FrameNumber() const { return frameNumber; }
    };
}

extern blackbox::Engine Engine;
