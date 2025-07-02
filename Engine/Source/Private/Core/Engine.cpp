#include "Engine.hpp"
#include <chrono>
#include "Stasis.h"

Stasis::Engine Engine;

void Stasis::Engine::Initialize()
{
    LogEngine->Trace("Initializing Engine...");

    /* Initialize the library */
    if (!glfwInit())
    {
        LogEngine->Error("Failed to initialize GLFW.");
        return;
    }

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    Window = glfwCreateWindow(720, 405, "Stasis Engine", nullptr, nullptr);
    if (!Window)
    {
        glfwTerminate();
        LogEngine->Error("Failed to create GLFW window.");
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(Window);
    

    IsInitialized = true;
}

void Stasis::Engine::Shutdown()
{
    LogEngine->Trace("Shutting Down Engine...");

    glfwTerminate();
}

void Stasis::Engine::Run()
{
    // Do not run engine if not successfully initialized
    if (!IsInitialized) return;
    
    auto PreviousTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(Window) || StopEngine)
    {
        const auto CurrentTime = std::chrono::high_resolution_clock::now();
        const float Elapsed = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(CurrentTime - PreviousTime).count());
        const float DeltaTime = Elapsed / 1000000.0f; // time in seconds
        const float FrameTime = Elapsed / 1000.0f; // time in milliseconds
        PreviousTime = CurrentTime;
        
        /* Poll for and process events */
        glfwPollEvents();

        // Do not draw if we are minimized
        if (StopRendering)
        {
            // Throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        Draw();
        LogEngine->Trace("FPS: {:.1f} ({}ms)", 1000.0f / FrameTime, FrameTime);
    }
}

void Stasis::Engine::Draw()
{
    FrameNumber++;
}


