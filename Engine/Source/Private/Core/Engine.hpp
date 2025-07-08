#pragma once

#include <memory>

struct SDL_Window;

namespace Stasis
{
    class VulkanRenderer;
    
    class Engine
    {
        bool stopRendering {false};
        bool isRunning {true};
        
        std::unique_ptr<VulkanRenderer> renderer {nullptr};
        
    public:
        void Initialize();
        void Run();
        void Shutdown();

        [[nodiscard]] VulkanRenderer& Renderer() const { return *renderer; }
    };
}

extern Stasis::Engine gEngine;
