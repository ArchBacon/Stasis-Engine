#pragma once

#include <ranges>

#include "Core/Types.hpp"
#include "Rendering/vk_types.h"
#include "VkBootstrap.h"

struct SDL_Window;

namespace Stasis
{
    struct DeletionQueue
    {
        std::deque<std::function<void()>> Deletors {};

        void PushFunction(std::function<void()>&& Function)
        {
            Deletors.push_back(Function);
        }

        void Flush()
        {
            // Reverse iterate the deletion queue to execute all the functions
            for (auto& Function : std::ranges::reverse_view(Deletors))
            {
                Function();
            }

            Deletors.clear();
        }
    };
    
    struct FrameData
    {
        VkCommandPool CommandPool {};
        VkCommandBuffer CommandBuffer {};
        VkSemaphore SwapchainSemaphore {};
        VkSemaphore RenderSemaphore {};
        VkFence RenderFence {};
        DeletionQueue DeletionQueue {};
    };

    constexpr unsigned int FRAME_OVERLAP = 2;
    
    class Engine
    {
        /** Vulkan */
        VkInstance Instance {}; // Vulkan library handle
        VkDebugUtilsMessengerEXT DebugMessenger {}; // Vulkan debug output handle
        VkPhysicalDevice SelectedGPU {}; // GPU selected as the default device
        VkDevice Device {}; // Vulkan device for commands
        VkSurfaceKHR Surface {}; // Vulkan window surface

        /** Vulkan Swapchain */
        VkSwapchainKHR SwapChain {};
        VkFormat SwapChainImageFormat {};
        std::vector<VkImage> SwapChainImages {};
        std::vector<VkImageView> SwapChainImageViews {};
        VkExtent2D SwapChainExtent {};

        VmaAllocator Allocator {};
        DeletionQueue MainDeletionQueue {};
        
        bool IsInitialized {false};
        bool StopRendering {false};
        bool IsRunning {true};
        unsigned int FrameNumber {0};
        int2 WindowExtent {1024, 576};

        SDL_Window* Window {nullptr};
        
    /** Vulkan public members */
    public:
        FrameData Frames[FRAME_OVERLAP];
        FrameData& GetCurrentFrame() { return Frames[FrameNumber % FRAME_OVERLAP]; }

        VkQueue GraphicsQueue {};
        uint32_t GraphicsQueueFamily {};
        
    public:
        void Initialize();
        void Run();
        void Draw();
        void Shutdown();

    private:
        /** Vulkan */
        void InitVulkan();
        void InitSwapChain();
        void InitCommands();
        void InitSyncStructures();

        /** Vulkan Swapchain */
        void CreateSwapChain(uint32_t Width, uint32_t Height);
        void DestroySwapChain();
    };
}

extern Stasis::Engine Engine;
