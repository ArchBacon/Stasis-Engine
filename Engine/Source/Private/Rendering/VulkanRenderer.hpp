#pragma once

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include "vk_types.h"
#include "vk_initializers.h"
#include <VkBootstrap.h>

#include <ranges>

namespace Stasis
{
    struct DeletionQueue
    {
        std::deque<std::function<void()>> deletors {};

        void PushFunction(std::function<void()>&& callback) {
            deletors.push_back(callback);
        }

        void Flush() {
            // Reverse iterate the deletion queue to execute all the functions
            for (auto& callback : std::ranges::reverse_view(deletors))
            {
                callback();
            }

            deletors.clear();
        }
    };
    
    struct FrameData
    {
        VkCommandPool commandPool {};
        VkCommandBuffer commandBuffer {};
        VkSemaphore swapchainSemaphore {};
        VkFence renderFence {};
        DeletionQueue deletionQueue {};
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

        DeletionQueue mainDeletionQueue {};

        // Allocators
        VmaAllocator allocator {};

        // Draw resources
        AllocatedImage drawImage {};
        VkExtent2D drawExtent {};
    
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

        void DrawBackground(VkCommandBuffer commandBuffer);
    };
}
