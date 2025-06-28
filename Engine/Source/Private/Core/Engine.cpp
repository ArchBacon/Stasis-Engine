#include "Engine.hpp"
#include <chrono>
#include <iostream>

DEFINE_LOG_CATEGORY(LogEngine);

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
    window = glfwCreateWindow(720, 405, "Stasis", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        LogEngine->Error("Failed to create GLFW window.");
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
}

void Stasis::Engine::Run()
{
    auto PreviousTime = std::chrono::high_resolution_clock::now();

    unsigned int FrameCount = 0;
    while (!glfwWindowShouldClose(window) && IsRunning)
    {
        const auto CurrentTime = std::chrono::high_resolution_clock::now();
        const float Elapsed = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(CurrentTime - PreviousTime).count());
        const float DeltaTime = Elapsed / 1000000.0f; // time in seconds
        const float FrameTime = Elapsed / 1000.0f; // time in milliseconds
        PreviousTime = CurrentTime;
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        
        FrameCount++;
        LogEngine->Trace("FPS: {:.1f} ({}ms)", 1000.0f / FrameTime, FrameTime);
    }
}

void Stasis::Engine::Shutdown()
{
    LogEngine->Trace("Shutting Down Engine...");

    glfwTerminate();
}
