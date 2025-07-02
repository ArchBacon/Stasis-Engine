#pragma once

#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"

namespace Stasis
{
    class Engine
    {
        bool IsInitialized {false};
        bool StopRendering {false};
        bool StopEngine {false};
        unsigned int FrameNumber {0};
        VkExtent2D WindowExtent {720, 405};
        
        GLFWwindow* Window {nullptr};
        
    public:
        void Initialize();
        void Run();
        void Draw();
        void Shutdown();
    };
}

extern Stasis::Engine Engine;
