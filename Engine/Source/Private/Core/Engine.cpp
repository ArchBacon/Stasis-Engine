#include "Engine.hpp"

#include <chrono>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include "Stasis.hpp"

Stasis::Engine gEngine;

void Stasis::Engine::Initialize()
{
    LogEngine->Trace("Initializing Engine...");

    SDL_Init(SDL_INIT_VIDEO);

    constexpr SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;
    window = SDL_CreateWindow("Stasis Engine", windowExtent.x, windowExtent.y, windowFlags);

    isInitialized = true;
}

void Stasis::Engine::Run()
{
    // Do not run engine if not successfully initialized
    if (!isInitialized) return;

    auto previousTime = std::chrono::high_resolution_clock::now();
    SDL_Event event;

    while (isRunning)
    {
        const auto currentTime = std::chrono::high_resolution_clock::now();
        const float elapsed = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime).count());
        const float deltaTime = elapsed / 1000000.0f; // time in seconds
        const float frameTime = elapsed / 1000.0f; // time in milliseconds
        previousTime = currentTime;

        // LogEngine->Trace("FPS: {:.1f} ({}ms)", 1000.0f / FrameTime, FrameTime);

        // Handle events in queue
        while (SDL_PollEvent(&event))
        {
            // Close the window when the user ALT-F4s or closes the window
            if (event.type == SDL_EVENT_QUIT)
            {
                isRunning = false;
            }

            // Pause rendering when the window is minimized or loses focus
            if (event.type == SDL_EVENT_WINDOW_MINIMIZED || event.type == SDL_EVENT_WINDOW_FOCUS_LOST)
            {
                stopRendering = true;
            }
            if (event.type == SDL_EVENT_WINDOW_RESTORED || event.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
            {
                stopRendering = false;
            }
        }

        // Do not draw if we are minimized
        if (stopRendering)
        {
            // Throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        frameNumber++;
    }
}

void Stasis::Engine::Shutdown()
{
    LogEngine->Trace("Shutting Down Engine...");

    if (isInitialized)
    {
        SDL_DestroyWindow(window);
    }
}
