#pragma once

#include "Core/Types.hpp"
#include "Rendering/vk_types.h"
#include "VkBootstrap.h"

struct SDL_Window;

namespace Stasis
{
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
        
        bool IsInitialized {false};
        bool StopRendering {false};
        bool IsRunning {true};
        unsigned int FrameNumber {0};
        int2 WindowExtent {1024, 576};

        SDL_Window* Window {nullptr};
        
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
