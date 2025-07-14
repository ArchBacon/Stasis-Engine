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

    std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadGltfMesh(VulkanRenderer* renderer, const std::filesystem::path& filePath);
}
