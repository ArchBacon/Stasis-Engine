#include "Engine.hpp"
#include <chrono>
#include <iostream>

DEFINE_LOG_CATEGORY(LogEngine);

Stasis::Engine Engine;

void Stasis::Engine::Initialize()
{
    LogEngine->Trace("Initializing Engine...");
}

void Stasis::Engine::Run()
{
    auto PreviousTime = std::chrono::high_resolution_clock::now();

    unsigned int FrameCount = 0;
    while (IsRunning)
    {
        const auto CurrentTime = std::chrono::high_resolution_clock::now();
        const float Elapsed = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(CurrentTime - PreviousTime).count());
        const float DeltaTime = Elapsed / 1000000.0f; // time in seconds
        const float FrameTime = Elapsed / 1000.0f; // time in milliseconds
        PreviousTime = CurrentTime;

        FrameCount++;
        LogEngine->Trace("FPS: {:.1f} ({}ms)", 1000.0f / FrameTime, FrameTime);
    }
}

void Stasis::Engine::Shutdown()
{
    std::cout << "Shutting down...\n";
    LogEngine->Trace("Shutting Down Engine...");
}
