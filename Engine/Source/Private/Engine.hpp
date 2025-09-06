#pragma once

#include <memory>

#include "Blackbox.hpp"
#include "EventBus.hpp"
#include "Input/InputMapping.hpp"
#include "Input/InputMappingContext.hpp"
#include "Input/InputValue.hpp"

namespace blackbox
{
    class Input;
    class Container;
    class Window;
    class FileIO;

    struct ExitEngineAction {};
    struct EngineContext final : InputMappingContext<EngineContext>
    {
        // ReSharper disable once CppPossiblyUnintendedObjectSlicing
        EngineContext() : InputMappingContext({
            InputMapping<ExitEngineAction> {
                {Keyboard::Escape}
            },
        }) {}
    };
    
    class BlackboxEngine
    {
        std::unique_ptr<Container> container {nullptr};
        EventBus* eventbus {nullptr};
        FileIO* fileIO {nullptr};
        Window* window {nullptr};
        Input* input {nullptr};

        bool stopRendering {false};
        bool isRunning {true};
        uint32_t frameNumber {0};

        float deltaTime {0.0f};
        float uptime {0.0f};
        
    public:
        void Initialize();
        void Run();
        void Shutdown();

        [[nodiscard]] float DeltaTime() const { return deltaTime; }
        [[nodiscard]] float Uptime() const { return uptime; } // How long the engine has een running in seconds
        [[nodiscard]] uint32_t FrameNumber() const { return frameNumber; }

    private:
        void RequestShutdown(const ShutdownEvent&) { isRunning = false; }
        void StopRendering(const Event&) { stopRendering = true; }
        void StartRendering(const Event&) { stopRendering = false; }

        void OnCloseAction(InputValue) { RequestShutdown({}); }
        
        void OnTriggeredAction(InputValue value) { LogEngine->Info("OnTriggeredAction {}", value.Get<float>()); }
    };
}

extern blackbox::BlackboxEngine Engine;
