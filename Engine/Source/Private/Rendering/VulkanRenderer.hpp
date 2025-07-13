#pragma once

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include "vk_types.h"
#include "vk_initializers.h"
#include "vk_descriptors.h"
#include <VkBootstrap.h>

#include <ranges>

#include "Core/Types.hpp"

namespace blackbox
{
    class VulkanRenderer;
    struct MeshAsset;

    struct DeletionQueue
    {
        std::deque<std::function<void()>> queue {};

        void Add(std::function<void()>&& callback)
        {
            queue.push_back(std::move(callback));
        }

        void Flush() {
            // Reverse iterate the deletion queue to execute all the functions
            for (auto& callback : std::ranges::reverse_view(queue))
            {
                callback();
            }

            queue.clear();
        }
    };
    
    struct FrameData
    {
        VkCommandPool commandPool {};
        VkCommandBuffer commandBuffer {};
        
        VkSemaphore swapchainSemaphore {};
        VkFence renderFence {};
        
        DeletionQueue deletionQueue {};
        DescriptorAllocatorGrowable frameDescriptor {};
    };

    struct ComputePushConstants
    {
        float4 data1;
        float4 data2;
        float4 data3;
        float4 data4;
    };

    struct ComputeEffect
    {
        const char* name {};
        VkPipeline pipeline {};
        VkPipelineLayout layout {};
        ComputePushConstants data {};
    };

    struct GPUSceneData
    {
        mat4 view {1.0f};
        mat4 proj {1.0f};
        mat4 viewproj {1.0f};
        float4 ambientColor {0.2f, 0.2f, 0.2f, 1.0f};
        float4 sunlightDirection {0.0f, 0.0f, 1.0f, 1.0f}; // W for sun power
        float4 sunlightColor {1.0f, 1.0f, 1.0f, 1.0f};
    };

    struct GLTFMetallicRoughness
    {
        MaterialPipeline opaquePipeline {};
        MaterialPipeline transparentPipeline {};
        VkDescriptorSetLayout materialLayout {};

        struct MaterialConstants
        {
            float4 colorFactors {1.0f};
            float4 metallicRoughnessFactors {0.0f};
            float4 extra[14] {}; // Padding, we need it anyway for uniform buffers (256 byte buffers)
        };

        struct MaterialResources
        {
            AllocatedImage colorImage {};
            VkSampler colorSampler {};
            AllocatedImage metallicRoughnessImage {};
            VkSampler metallicRoughnessSampler {};
            VkBuffer dataBuffer {};
            uint32_t dataBufferOffset {0};
        };

        DescriptorWriter writer {};

        void BuildPipelines(VulkanRenderer* renderer);
        void ClearResources(VkDevice device);

        MaterialInstance WriteMaterial(VkDevice device, MaterialPass pass, const MaterialResources& resources, DescriptorAllocatorGrowable descriptorAllocator);
    };

    constexpr uint8_t FRAME_OVERLAP = 3;
    
    class VulkanRenderer
    {
        friend struct GLTFMetallicRoughness;
        
        uint32_t frameNumber {0};
        VkExtent2D windowExtent {1024, 576};
        SDL_Window* window {nullptr};
        bool resizeRequested {false};

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

        DeletionQueue deletionQueue {};

        // Allocators
        VmaAllocator allocator {};

        // Draw resources
        AllocatedImage drawImage {};
        AllocatedImage depthImage {};
        VkExtent2D drawExtent {};
        float renderScale = 1.0f;

        // Descriptors
        DescriptorAllocatorGrowable globalDescriptorAllocator {};
        VkDescriptorSet drawImageDescriptors {};
        VkDescriptorSetLayout drawImageDescriptorLayout {};

        VkPipeline gradientPipeline {};
        VkPipelineLayout gradientPipelineLayout {};

        // Immediate submit structured
        VkFence immediateFence {};
        VkCommandBuffer immediateCommandBuffer {};
        VkCommandPool immediateCommandPool {};

        std::vector<ComputeEffect> backgroundEffects {};
        int currentComputeEffectIndex {0};

        VkPipelineLayout meshPipelineLayout {};
        VkPipeline meshPipeline {};

        std::vector<std::shared_ptr<MeshAsset>> testMeshes {};

        GPUSceneData sceneData {};
        VkDescriptorSetLayout gpuSceneDataDescriptorLayout {};

        AllocatedImage whiteImage {};
        AllocatedImage blackImage {};
        AllocatedImage greyImage {};
        AllocatedImage checkerboardImage {};

        VkSampler defaultSamplerLinear {};
        VkSampler defaultSamplerNearest {};

        VkDescriptorSetLayout singleImageDescriptorLayout {};

        MaterialInstance defaultData {};
        GLTFMetallicRoughness metallicRoughnessMaterial {};
        
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

        GPUMeshBuffers UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);
        
    private:
        void InitVulkan();
        void InitSwapchain();
        void InitCommands();
        void InitSyncStructures();
        void InitDescriptors();
        void InitPipelines();
        void InitBackgroundPipelines();
        void InitMeshPipeline();
        void InitDefaultData();

        void CreateSwapchain(uint32_t width, uint32_t height);
        void DestroySwapchain();
        void ResizeSwapchain();

        void DrawBackground(VkCommandBuffer commandBuffer);
        void DrawImGui(VkCommandBuffer commandBuffer, VkImageView targetImageView);
        void DrawGeometry(VkCommandBuffer commandBuffer);
        
        void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& callback);
        void InitImGui();

        AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
        void DestroyBuffer(const AllocatedBuffer& buffer);

        AllocatedImage CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
        AllocatedImage CreateImage(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
        void DestroyImage(const AllocatedImage& image);
    };
}
