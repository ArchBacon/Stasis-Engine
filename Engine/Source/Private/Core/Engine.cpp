#include "Engine.hpp"

#include <chrono>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include "Blackbox.hpp"

blackbox::Engine Engine;

void blackbox::Engine::Initialize()
{
    LogEngine->Trace("Initializing Engine...");

    SDL_Init(SDL_INIT_VIDEO);

    constexpr SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;
    window = SDL_CreateWindow("Blackbox", windowExtent.x, windowExtent.y, windowFlags);
    SDL_Surface* iconSurface = SDL_LoadBMP("Content/Icon64x64.bmp");
    SDL_SetWindowIcon(window, iconSurface);
    SDL_DestroySurface(iconSurface);
}

void blackbox::Engine::Run()
{
    auto previousTime = std::chrono::high_resolution_clock::now();
    SDL_Event event;

    while (isRunning)
    {
        const auto currentTime = std::chrono::high_resolution_clock::now();
        const float elapsed = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime).count());
        const float deltaTime = elapsed / 1000000.0f; // time in seconds
        const float frameTime = elapsed / 1000.0f; // time in milliseconds
        previousTime = currentTime;

        // Handle events in a queue
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

void blackbox::Engine::Shutdown()
{
    LogEngine->Trace("Shutting Down Engine...");

    SDL_DestroyWindow(window);
}
