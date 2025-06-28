#pragma once

#include "Core/Logger.hpp"
#include "GLFW/glfw3.h"

DECLARE_LOG_CATEGORY(LogEngine)

namespace Stasis
{
    class Engine
    {
        bool IsRunning {true};
        GLFWwindow* window {nullptr};
        
    public:
        void Initialize();
        void Run();
        void Shutdown();
    };
}

extern Stasis::Engine Engine;
