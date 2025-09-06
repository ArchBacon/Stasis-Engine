#include "Engine.hpp"

#include <chrono>
#include <thread>

#include <SDL3/SDL_events.h>
#include "Blackbox.hpp"
#include "DependencyInjection.hpp"
#include "FileIO.hpp"
#include "Window.hpp"
#include "Helpers/SDL3EventHelper.hpp"
#include "Input/Input.hpp"

blackbox::BlackboxEngine Engine;

void blackbox::BlackboxEngine::Initialize()
{
    LogEngine->Trace("Initializing Engine...");

    SDL_Init(SDL_INIT_VIDEO);
    
    // Populate the DI container
    container = std::make_unique<Container>();
    eventbus = container->Register<EventBus>();
    fileIO = container->Register<FileIO>();
    window = container->Register<Window, EventBus&>(1024, 576, "Blackbox", "Content/Icon64x64.bmp");
    input = container->Register<Input, EventBus&>();

    // Subscribe to events and assign callbacks
    eventbus->Subscribe<ShutdownEvent>(this, &BlackboxEngine::RequestShutdown);
    eventbus->Subscribe<WindowMinimizedEvent>(this, &BlackboxEngine::StopRendering);
    eventbus->Subscribe<WindowFocusLostEvent>(this, &BlackboxEngine::StopRendering);
    eventbus->Subscribe<WindowRestoredEvent>(this, &BlackboxEngine::StartRendering);
    eventbus->Subscribe<WindowFocusGainedEvent>(this, &BlackboxEngine::StartRendering);

    input->AddContext<EngineContext>();
}

void blackbox::BlackboxEngine::Run()
{
    auto previousTime = std::chrono::high_resolution_clock::now();
    SDL_Event event;

    // TODO: Move to editor play window once it's in, since this shouldn't be default for every project
    auto& exitEvent = input->GetAction<ExitEngineAction>();
    exitEvent.OnStarted(this, &BlackboxEngine::OnCloseAction);
    
    while (isRunning)
    {
        const auto currentTime = std::chrono::high_resolution_clock::now();
        const float elapsed = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime).count());
        deltaTime = elapsed / 1000000.0f; // time in seconds
        uptime += deltaTime;
        const float frameTime = elapsed / 1000.0f; // time in milliseconds
        previousTime = currentTime;

        while (SDL_PollEvent(&event))
        {
            SDL3ToBlackBoxEvent::Broadcast(event, *eventbus);
        }

        // Do not draw if we are minimized
        if (stopRendering)
        {
            // Throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        eventbus->Broadcast(TickEvent{.deltaTime = deltaTime});
        window->SwapBuffers();
        
        frameNumber++;
    }
}

void blackbox::BlackboxEngine::Shutdown()
{
    LogEngine->Trace("Shutting Down Engine...");
    LogEngine->Info("Engine uptime: {}s", Uptime());

    SDL_Quit();
}
