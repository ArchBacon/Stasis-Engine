#pragma once

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include "vk_types.h"
#include "vk_initializers.h"
#include <VkBootstrap.h>

namespace Stasis
{
    struct FrameData
    {
        VkCommandPool commandPool {};
        VkCommandBuffer commandBuffer {};
        VkSemaphore swapchainSemaphore {};
        VkFence renderFence {};
    };

    constexpr uint8_t FRAME_OVERLAP = 3;
    
    class VulkanRenderer
    {
        uint32_t frameNumber {0};
        VkExtent2D windowExtent {1024, 576};
        SDL_Window* window {nullptr};

        VkInstance instance {};
        VkDebugUtilsMessengerEXT debugMessenger {};
        VkPhysicalDevice selectedGPU {};
        VkDevice device {};
        VkSurfaceKHR surface {};
        
        VkSwapchainKHR swapchain {};
        VkFormat swapchainImageFormat {};
        std::vector<VkImage> swapchainImages {};
        std::vector<VkImageView> swapchainImageViews {};
        VkExtent2D swapchainExtent {};

        FrameData frames[FRAME_OVERLAP];
        FrameData& GetCurrentFrame() { return frames[frameNumber % FRAME_OVERLAP]; }
        std::vector<VkSemaphore> renderSemaphores {};

        VkQueue graphicsQueue {};
        uint32_t graphicsQueueFamily {};
    
    public:
        VulkanRenderer();
        ~VulkanRenderer();

        VulkanRenderer(const VulkanRenderer&)            = delete;
        VulkanRenderer(VulkanRenderer&&)                 = delete;
        VulkanRenderer& operator=(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(VulkanRenderer&&)      = delete;

    protected:
        void Initialize();
        void Shutdown();

    public:
        void Draw();
        
    private:
        void InitVulkan();
        void InitSwapchain();
        void InitCommands();
        void InitSyncStructures();

        void CreateSwapchain(uint32_t width, uint32_t height);
        void DestroySwapchain();
    };
}
