#include "VulkanRenderer.hpp"
#include <SDL3/SDL_vulkan.h>

Stasis::VulkanRenderer::VulkanRenderer()
{
    VulkanRenderer::Initialize();
}

Stasis::VulkanRenderer::~VulkanRenderer()
{
    VulkanRenderer::Shutdown();
}

void Stasis::VulkanRenderer::Initialize()
{
    SDL_Init(SDL_INIT_VIDEO);

    constexpr SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;
    window = SDL_CreateWindow("Stasis Engine", static_cast<int>(windowExtent.width), static_cast<int>(windowExtent.height), windowFlags);
}

void Stasis::VulkanRenderer::Shutdown()
{
    SDL_DestroyWindow(window);
}

void Stasis::VulkanRenderer::Draw()
{
    frameNumber++;
}
