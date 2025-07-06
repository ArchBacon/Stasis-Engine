#pragma once

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>

namespace Stasis
{
    class VulkanRenderer
    {
        uint32_t frameNumber {0};
        VkExtent2D windowExtent {1024, 576};

        SDL_Window* window {nullptr};
    
    public:
        VulkanRenderer();
        ~VulkanRenderer();

        VulkanRenderer(const VulkanRenderer&)            = delete;
        VulkanRenderer(VulkanRenderer&&)                 = delete;
        VulkanRenderer& operator=(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(VulkanRenderer&&)      = delete;

        void Draw();

    protected:
        void Initialize();
        void Shutdown();
    };
}
