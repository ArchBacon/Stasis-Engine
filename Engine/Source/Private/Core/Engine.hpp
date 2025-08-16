#pragma once

#include <memory>

#include "Core/Types.hpp"

namespace blackbox::editor
{
    class Editor;
}

namespace blackbox::graphics
{
    class GlRenderer;
}

namespace blackbox
{
    class Window;
    class FileIO;
    
    class Engine
    {
        std::unique_ptr<editor::Editor> editor {nullptr};
        std::unique_ptr<Window> window {nullptr};
        std::unique_ptr<graphics::GlRenderer> renderer {nullptr};
        std::unique_ptr<FileIO> fileIO {nullptr};
        
        bool stopRendering {false};
        bool isRunning {true};
        uint32_t frameNumber {0};

        float deltaTime {0.0f};
        float uptime {0.0f};
        
    public:
        void Initialize();
        void Run();
        void Shutdown();

        [[nodiscard]] editor::Editor& Editor() const { return *editor; }
        [[nodiscard]] Window& Window() const { return *window; }
        [[nodiscard]] graphics::GlRenderer& Renderer() const { return *renderer; }
        [[nodiscard]] FileIO& FileIO() const { return *fileIO; }
        
        [[nodiscard]] float DeltaTime() const { return deltaTime; }
        // How long the engine has een running in seconds
        [[nodiscard]] float Uptime() const { return uptime; } 
        [[nodiscard]] uint32_t FrameNumber() const { return frameNumber; }
    };
}

extern blackbox::Engine Engine;
