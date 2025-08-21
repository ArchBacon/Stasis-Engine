#pragma once

#include <memory>
#include "Types.hpp"

namespace blackbox
{
    class Container;

    class BlackboxEngine
    {
        std::unique_ptr<Container> container {nullptr};
        
        bool stopRendering {false};
        bool isRunning {true};
        uint32_t frameNumber {0};

        float deltaTime {0.0f};
        float uptime {0.0f};
        
    public:
        void Initialize();
        void Run();
        void Shutdown();

        [[nodiscard]] Container& Container() const { return *container; }
        
        [[nodiscard]] float DeltaTime() const { return deltaTime; }
        [[nodiscard]] float Uptime() const { return uptime; } // How long the engine has een running in seconds
        [[nodiscard]] uint32_t FrameNumber() const { return frameNumber; }
    };
}

extern blackbox::BlackboxEngine Engine;
