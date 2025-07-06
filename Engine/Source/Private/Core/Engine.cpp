#include "Engine.hpp"

#include <chrono>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include "Stasis.hpp"

Stasis::Engine Engine;

void Stasis::Engine::Initialize()
{
    LogEngine->Trace("Initializing Engine...");

    SDL_Init(SDL_INIT_VIDEO);

    constexpr SDL_WindowFlags WindowFlags = SDL_WINDOW_VULKAN;
    Window = SDL_CreateWindow("Stasis Engine", WindowExtent.x, WindowExtent.y, WindowFlags);
    
    IsInitialized = true;
}

void Stasis::Engine::Run()
{
    // Do not run engine if not successfully initialized
    if (!IsInitialized) return;
    
    auto PreviousTime = std::chrono::high_resolution_clock::now();
    SDL_Event Event;
    
    while (IsRunning)
    {
        const auto CurrentTime = std::chrono::high_resolution_clock::now();
        const float Elapsed = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(CurrentTime - PreviousTime).count());
        const float DeltaTime = Elapsed / 1000000.0f; // time in seconds
        const float FrameTime = Elapsed / 1000.0f; // time in milliseconds
        PreviousTime = CurrentTime;

        // LogEngine->Trace("FPS: {:.1f} ({}ms)", 1000.0f / FrameTime, FrameTime);

        // Handle events in queue
        while (SDL_PollEvent(&Event))
        {
            // Close the window when the user ALT-F4s or closes the window
            if (Event.type == SDL_EVENT_QUIT)
            {
                IsRunning = false;
            }

            // Pause rendering when the window is minimized or loses focus
            if (Event.type == SDL_EVENT_WINDOW_MINIMIZED || Event.type == SDL_EVENT_WINDOW_FOCUS_LOST)
            {
                StopRendering = true;
            }
            if (Event.type == SDL_EVENT_WINDOW_RESTORED || Event.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
            {
                StopRendering = false;
            }
        }
        
        // Do not draw if we are minimized
        if (StopRendering)
        {
            // Throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        FrameNumber++;
    }
}

void Stasis::Engine::Shutdown()
{
    LogEngine->Trace("Shutting Down Engine...");

    if (IsInitialized)
    {
        SDL_DestroyWindow(Window);
    }
}
