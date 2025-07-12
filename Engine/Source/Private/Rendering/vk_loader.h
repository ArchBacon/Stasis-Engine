#pragma once

#include "vk_types.h"
#include <unordered_map>
#include <filesystem>

#include "VulkanRenderer.hpp"

namespace blackbox
{
    class VulkanRenderer;
    
    struct GeoSurface
    {
        uint32_t startIndex {};
        uint32_t count {};
    };

    struct MeshAsset
    {
        std::string name {};
        std::vector<GeoSurface> surfaces {};
        GPUMeshBuffers meshBuffers {};
    };

    std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadGltfMesh(VulkanRenderer* renderer, std::filesystem::path filePath);
}
