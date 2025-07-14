#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <span>
#include <array>
#include <functional>
#include <deque>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "Blackbox.hpp"
#include "Core/Types.hpp"

namespace blackbox
{
    #define VK_CHECK(x)                                                             \
    do {                                                                            \
        VkResult err = x;                                                           \
        if (err) {                                                                  \
            LogRenderer->Error("Detected Vulkan error: {}", string_VkResult(err));  \
            abort();                                                                \
        }                                                                           \
    } while (0)

    struct AllocatedImage
    {
        VkImage image {};
        VkImageView imageView {};
        VmaAllocation allocation {};
        VkExtent3D imageExtent {};
        VkFormat imageFormat {};
    };

    struct AllocatedBuffer
    {
        VkBuffer buffer {};
        VmaAllocation allocation {};
        VmaAllocationInfo info {};
    };

    struct Vertex
    {
        float3 position {};
        float uvX {};
        float3 normal {};
        float uvY {};
        float4 color {};
    };

    // Holds the resources needed for a mesh
    struct GPUMeshBuffers
    {
        AllocatedBuffer indexBuffer {};
        AllocatedBuffer vertexBuffer {};
        VkDeviceAddress vertexBufferAddress {};
    };

    // Push constants for our mesh object draws
    struct GPUDrawPushConstants
    {
        mat4 worldMatrix {};
        VkDeviceAddress vertexBuffer {};
    };

    enum class MaterialPass : uint8_t
    {
        MainColor,
        Transparent,
        Other,
    };

    struct MaterialPipeline
    {
        VkPipeline pipeline {};
        VkPipelineLayout layout {};
    };

    struct MaterialInstance
    {
        MaterialPipeline* pipeline {nullptr};
        VkDescriptorSet materialSet {};
        MaterialPass passType {MaterialPass::MainColor};
    };

    struct DrawContext;

    // Base class for a renderable dynamic object
    class IRenderable
    {
        virtual void Draw(const mat4& topMatrix, DrawContext& ctx) = 0;
    };

    // Implementation of a drawable scene node
    // The scene node can hold children and will also keep a transform to propagate to them
    struct Node : public IRenderable
    {
        // Parent pointer must be a weak pointer to avoid circular dependencies
        std::weak_ptr<Node> parent {};
        std::vector<std::shared_ptr<Node>> children {};

        mat4 localTransform {1.0f};
        mat4 worldTransform {1.0f};

        void RefreshTransform(const mat4& parentMatrix)
        {
            worldTransform = parentMatrix * localTransform;
            for (const auto& child : children)
            {
                child->RefreshTransform(worldTransform);
            } 
        }

        virtual void Draw(const mat4& topMatrix, DrawContext& context)
        {
            // Draw children
            for (const auto& child : children)
            {
                child->Draw(topMatrix, context);
            } 
        }
    };
}
