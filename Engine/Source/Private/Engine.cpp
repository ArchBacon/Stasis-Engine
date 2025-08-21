﻿#include "Engine.hpp"

#include <chrono>
#include <thread>

#include <SDL3/SDL_events.h>
#include "Blackbox.hpp"
#include "DependencyInjection.hpp"
#include "FileIO.hpp"
#include "Window.hpp"

blackbox::BlackboxEngine Engine;

void blackbox::BlackboxEngine::Initialize()
{
    LogEngine->Trace("Initializing Engine...");

    SDL_Init(SDL_INIT_VIDEO);
    
    container = std::make_unique<blackbox::Container>();
    container->Register<FileIO>();
    container->Register<Window>(1024, 576, "Blackbox", "Content/Icon64x64.bmp");
}

void blackbox::BlackboxEngine::Run()
{
    auto previousTime = std::chrono::high_resolution_clock::now();
    SDL_Event event;

    while (isRunning)
    {
        const auto currentTime = std::chrono::high_resolution_clock::now();
        const float elapsed = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime).count());
        deltaTime = elapsed / 1000000.0f; // time in seconds
        uptime += deltaTime;
        const float frameTime = elapsed / 1000.0f; // time in milliseconds
        previousTime = currentTime;

        // Handle events in a queue
        while (SDL_PollEvent(&event))
        {
            // TODO: handle these events in an event bus or something, so that I don't have a gigantic list of events for everything here.
            
            // Close the window when the user ALT-F4s or closes the window
            if (event.type == SDL_EVENT_QUIT)
            {
                isRunning = false;
            }

            // Close on ESC key press
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
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

            // Re-set viewport size on window resized
            if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                // window.OnWindowResized(event.window.data1, event.window.data2);
            }
        }

        // Do not draw if we are minimized
        if (stopRendering)
        {
            // Throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // window.SwapBuffers();
        
        frameNumber++;
    }
}

void blackbox::BlackboxEngine::Shutdown()
{
    LogEngine->Trace("Shutting Down Engine...");
    LogEngine->Info("Engine uptime: {}s", Uptime());

    SDL_Quit();
}
