#include "Engine.hpp"
#include <chrono>
#include <iostream>

Stasis::Engine Engine;

void Stasis::Engine::Initialize()
{
    std::cout << "Starting Engine...\n";
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

        // Log frame info every 60 frames
        FrameCount++;
        std::cout << "Frame: " << FrameTime << "ms\n";
    }
}

void Stasis::Engine::Shutdown()
{
    std::cout << "Shutting down...\n";
}
