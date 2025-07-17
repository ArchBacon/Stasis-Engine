#pragma once

#include "vk_types.h"
#include <unordered_map>
#include <filesystem>

#include "VulkanRenderer.hpp"

namespace blackbox
{
    class VulkanRenderer;

    struct GLTFMaterial
    {
        MaterialInstance data {};
    };
    
    struct GeoSurface
    {
        uint32_t startIndex {};
        uint32_t count {};
        std::shared_ptr<GLTFMaterial> material {nullptr};
    };

    struct MeshAsset
    {
        std::string name {};
        std::vector<GeoSurface> surfaces {};
        GPUMeshBuffers meshBuffers {};
    };

    // std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadGltfMesh(VulkanRenderer* renderer, const std::filesystem::path& filePath);

    struct LoadedGLTF : public IRenderable
    {
        // Storage for all the data on a given GLTF file
        std::unordered_map<std::string, std::shared_ptr<MeshAsset>> meshes {};
        std::unordered_map<std::string, std::shared_ptr<Node>> nodes {};
        std::unordered_map<std::string, AllocatedImage> images {};
        std::unordered_map<std::string, std::shared_ptr<GLTFMaterial>> materials {};

        // Nodes that don't have a parent, for iterating through the file in tree order
        std::vector<std::shared_ptr<Node>> topNodes {};
        std::vector<VkSampler> samplers {};

        DescriptorAllocatorGrowable descriptorPool {};
        AllocatedBuffer materialDataBuffer {};
        VulkanRenderer* creator {nullptr};

        ~LoadedGLTF() { ClearAll(); }

        virtual void Draw(const mat4& topMatrix, DrawContext& ctx) override;

    private:
        void ClearAll();
    };

    std::optional<std::shared_ptr<LoadedGLTF>> LoadGLTF(VulkanRenderer* renderer, const std::filesystem::path& filePath);
}
