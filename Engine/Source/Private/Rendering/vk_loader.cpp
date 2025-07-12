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
    std::filesystem::path filePath
)
{
    // ------------- LOAD GLTF START -------------
    LogRenderer->Trace("Loading GLTF file: {}", filePath.string());

    auto gltfFile = fastgltf::GltfDataBuffer::FromPath(filePath);
    if (!bool(gltfFile))
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
    // ------------- LOAD GLTF END -------------

    // ------------- LOAD MESH START -------------
    std::vector<std::shared_ptr<MeshAsset>> meshes{};

    // Use the same vectors for all meshes so that the memory doesn't reallocate as often
    std::vector<uint32_t> indices{};
    std::vector<Vertex> vertices{};
    for (fastgltf::Mesh& mesh : gltf.meshes)
    {
        MeshAsset newMesh;

        newMesh.name = mesh.name;

        // clear the mesh arrays each mesh, we don't want to merge them by error
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

                fastgltf::iterateAccessor<uint32_t>(gltf, indexAccessor, [&](uint32_t index)
                {
                    indices.push_back(index + (uint32_t)initialVertex);
                });
            }

            // load vertex positions
            {
                fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->accessorIndex];
                vertices.resize(vertices.size() + posAccessor.count);

                fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor, [&](glm::vec3 v, size_t index)
                {
                    Vertex newVertex{};
                    newVertex.position = v;
                    newVertex.normal = {1, 0, 0};
                    newVertex.color = glm::vec4{1.f};
                    newVertex.uvX = 0;
                    newVertex.uvY = 0;
                    vertices[initialVertex + index] = newVertex;
                });
            }

            // load vertex normals
            auto normals = p.findAttribute("NORMAL");
            if (normals != p.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).accessorIndex], [&](glm::vec3 v, size_t index)
                {
                    vertices[initialVertex + index].normal = v;
                });
            }

            // load UVs
            auto uv = p.findAttribute("TEXCOORD_0");
            if (uv != p.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).accessorIndex], [&](glm::vec2 v, size_t index)
                {
                    vertices[initialVertex + index].uvX = v.x;
                    vertices[initialVertex + index].uvY = v.y;
                });
            }

            // load vertex colors
            auto colors = p.findAttribute("COLOR_0");
            if (colors != p.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).accessorIndex], [&](glm::vec4 v, size_t index)
                {
                    vertices[initialVertex + index].color = v;
                });
            }
            newMesh.surfaces.push_back(newSurface);
        }

        // display the vertex normals
        constexpr bool overrideColors = true;
        if (overrideColors)
        {
            for (Vertex& vtx : vertices)
            {
                vtx.color = glm::vec4(vtx.normal, 1.f);
            }
        }
        newMesh.meshBuffers = renderer->UploadMesh(indices, vertices);

        meshes.emplace_back(std::make_shared<MeshAsset>(std::move(newMesh)));
    }
    // ------------- LOAD MESH END -------------

    return meshes;
}
