#include "vk_loader.h"

#include "stb_image.h"
#include "VulkanRenderer.hpp"
#include "vk_initializers.h"
#include "vk_types.h"
// #include <glm/gtx/quaternion.hpp>

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include "fastgltf/core.hpp"

std::optional<std::vector<std::shared_ptr<blackbox::MeshAsset>>> blackbox::LoadGltfMesh(
    VulkanRenderer* renderer,
    const std::filesystem::path& filePath
)
{
    // ------------- LOAD GLTF -------------
    LogRenderer->Trace("Loading GLTF file: {}", filePath.string());

    auto gltfFile = fastgltf::GltfDataBuffer::FromPath(filePath);
    if (!static_cast<bool>(gltfFile))
    {
        LogRenderer->Error("Failed to load GLTF file: {}. Reason: {}", filePath.string(), fastgltf::getErrorMessage(gltfFile.error()));
    }

    constexpr auto gltfOptions = fastgltf::Options::LoadExternalBuffers;

    fastgltf::Asset gltf{};
    fastgltf::Parser parser{};

    auto asset = parser.loadGltfBinary(gltfFile.get(), filePath.parent_path(), gltfOptions);
    if (asset.error() != fastgltf::Error::None)
    {
        LogRenderer->Error("Failed to parse GLTF file: {}. Reason: {}", filePath.string(), fastgltf::getErrorMessage(asset.error()));
    }

    gltf = std::move(asset.get());

    // ------------- LOAD MESH -------------
    std::vector<std::shared_ptr<MeshAsset>> meshes{};

    // Use the same vectors for all meshes so that the memory doesn't reallocate as often
    std::vector<uint32_t> indices{};
    std::vector<Vertex> vertices{};
    for (fastgltf::Mesh& mesh : gltf.meshes)
    {
        MeshAsset newMesh;

        newMesh.name = mesh.name;

        // Clear the mesh arrays each mesh, we don't want to merge them by error
        indices.clear();
        vertices.clear();

        for (auto&& p : mesh.primitives)
        {
            GeoSurface newSurface;
            newSurface.startIndex = static_cast<uint32_t>(indices.size());
            newSurface.count = static_cast<uint32_t>(gltf.accessors[p.indicesAccessor.value()].count);

            size_t initialVertex = vertices.size();

            // load indexes
            {
                fastgltf::Accessor& indexAccessor = gltf.accessors[p.indicesAccessor.value()];
                indices.reserve(indices.size() + indexAccessor.count);

                fastgltf::iterateAccessor<uint32_t>(gltf, indexAccessor, [&](const uint32_t index)
                {
                    indices.push_back(index + static_cast<uint32_t>(initialVertex));
                });
            }

            // load vertex positions
            {
                fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->accessorIndex];
                vertices.resize(vertices.size() + posAccessor.count);

                fastgltf::iterateAccessorWithIndex<float3>(gltf, posAccessor, [&](const float3 vertex, const size_t index)
                {
                    const Vertex newVertex
                    {
                        .position = vertex,
                        .uvX = 0,
                        .normal = {1, 0, 0},
                        .uvY = 0,
                        .color = float4{1.f},
                    };
                    vertices[initialVertex + index] = newVertex;
                });
            }

            // load vertex normals
            auto normals = p.findAttribute("NORMAL");
            if (normals != p.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<float3>(gltf, gltf.accessors[normals->accessorIndex], [&](const float3 vertex, const size_t index)
                {
                    vertices[initialVertex + index].normal = vertex;
                });
            }

            // load UVs
            auto uv = p.findAttribute("TEXCOORD_0");
            if (uv != p.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<float2>(gltf, gltf.accessors[uv->accessorIndex], [&](const float2 vertex, const size_t index)
                {
                    vertices[initialVertex + index].uvX = vertex.x;
                    vertices[initialVertex + index].uvY = vertex.y;
                });
            }

            // load vertex colors
            auto colors = p.findAttribute("COLOR_0");
            if (colors != p.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<float4>(gltf, gltf.accessors[colors->accessorIndex], [&](const float4 vertex, const size_t index)
                {
                    vertices[initialVertex + index].color = vertex;
                });
            }
            newMesh.surfaces.push_back(newSurface);
        }

        // display the vertex normals
        constexpr bool overrideColors = true;
        if (overrideColors)
        {
            for (Vertex& vertex : vertices)
            {
                vertex.color = float4(vertex.normal, 1.f);
            }
        }
        
        newMesh.meshBuffers = renderer->UploadMesh(indices, vertices);
        meshes.emplace_back(std::make_shared<MeshAsset>(std::move(newMesh)));
    }

    return meshes;
}
