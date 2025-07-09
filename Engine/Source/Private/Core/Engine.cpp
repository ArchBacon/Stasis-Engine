#include "Engine.hpp"

#include <chrono>
#include "Stasis.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_vulkan.h"
#include "Rendering/VulkanRenderer.hpp"

Stasis::Engine gEngine;

void Stasis::Engine::Initialize()
{
    LogEngine->Trace("Initializing Engine...");

    renderer = std::make_unique<VulkanRenderer>();
}

void Stasis::Engine::Run()
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

       // LogEngine->Trace("FPS: {:.1f} ({}ms)", 1000.0f / frameTime, frameTime);

        // Handle events in the queue
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

            ImGui_ImplSDL3_ProcessEvent(&event);
        }

        // Do not draw if we are minimized
        if (stopRendering)
        {
            // Throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        renderer->Draw();
    }
}

void Stasis::Engine::Shutdown()
{
    LogEngine->Trace("Shutting Down Engine...");
}
