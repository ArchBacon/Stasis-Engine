#pragma once

#include <ranges>
#include "Rendering/vk_types.h"
#include "VkBootstrap.h"
#include "Rendering/vk_descriptors.h"
#include "imgui/imgui.h"

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
    // ---------------- VULKAN START ---------------- 
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

        // Draw resources
        AllocatedImage DrawImage {};
        VkExtent2D DrawExtent {};

        // Allocators
        VmaAllocator Allocator {};
        
    // ---------------- VULKAN END ----------------
        
        DeletionQueue MainDeletionQueue {};
        
        bool IsInitialized {false};
        bool StopRendering {false};
        bool IsRunning {true};
        unsigned int FrameNumber {0};
        VkExtent2D WindowExtent {1024, 576};

        SDL_Window* Window {nullptr};
        
    // ---------------- VULKAN START ----------------
    public:
        FrameData Frames[FRAME_OVERLAP];
        FrameData& GetCurrentFrame() { return Frames[FrameNumber % FRAME_OVERLAP]; }

        VkQueue GraphicsQueue {};
        uint32_t GraphicsQueueFamily {};

        DescriptorAllocator GlobalDescriptorAllocator {};
        VkDescriptorSet DrawImageDescriptors {};
        VkDescriptorSetLayout DrawImageDescriptorLayout {};

        VkPipeline GradientPipeline {};
        VkPipelineLayout GradientPipelineLayout {};
    // ---------------- VULKAN END ----------------
        
    public:
        void Initialize();
        void Run();
        void Draw();
        void DrawBackground(VkCommandBuffer CommandBuffer);
        void Shutdown();

    private:
    // ---------------- VULKAN START ----------------
        void InitVulkan();
        void InitSwapChain();
        void InitCommands();
        void InitSyncStructures();
        
        void CreateSwapChain(uint32_t Width, uint32_t Height);
        void DestroySwapChain();

        void InitDescriptors();

        void InitPipelines();
        void InitBackgroundPipelines();
    // ---------------- VULKAN END ----------------
    };
}

extern Stasis::Engine Engine;
