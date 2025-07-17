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

namespace blackbox
{
    std::optional<AllocatedImage> LoadGLTFImage(VulkanRenderer* renderer, fastgltf::Asset& asset, fastgltf::Image& image)
    {
        AllocatedImage newImage {};

        int width, height, nrChannels;

        std::visit(
            fastgltf::visitor {
                [](auto& arg) {},
                [&](fastgltf::sources::URI& filePath) {
                    assert(filePath.fileByteOffset == 0);
                    assert(filePath.uri.isLocalPath());

                    const std::string path(filePath.uri.path().begin(), filePath.uri.path().end());
                    if (unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4))
                    {
                        VkExtent3D imageSize;
                        imageSize.width  = width;
                        imageSize.height = height;
                        imageSize.depth  = 1;

                        newImage = renderer->CreateImage(data, imageSize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, true);

                        stbi_image_free(data);
                    }
                },
                [&](fastgltf::sources::Array& array) {
                    if (unsigned char* data = stbi_load_from_memory(reinterpret_cast<stbi_uc const*>(array.bytes.data()), static_cast<int>(array.bytes.size()), &width, &height, &nrChannels, 4))
                    {
                        VkExtent3D imageSize;
                        imageSize.width  = width;
                        imageSize.height = height;
                        imageSize.depth  = 1;

                        newImage = renderer->CreateImage(data, imageSize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, true);

                        stbi_image_free(data);
                    }
                },
                [&](fastgltf::sources::BufferView& view) {
                    auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                    auto& buffer     = asset.buffers[bufferView.bufferIndex];

                    std::visit(
                        fastgltf::visitor {
                            [](auto& arg) {},
                            [&](fastgltf::sources::Array& array) {
                                if (unsigned char* data = stbi_load_from_memory(
                                    reinterpret_cast<stbi_uc const*>(array.bytes.data() + bufferView.byteOffset),
                                    static_cast<int>(bufferView.byteLength),
                                    &width,
                                    &height,
                                    &nrChannels,
                                    4))
                                {
                                    VkExtent3D imageSize;
                                    imageSize.width  = width;
                                    imageSize.height = height;
                                    imageSize.depth  = 1;

                                    newImage = renderer->CreateImage(data, imageSize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, true);

                                    stbi_image_free(data);
                                }
                            }},
                        buffer.data);
                },
            },
            image.data);
        if (newImage.image == VK_NULL_HANDLE)
        {
            return {};
        }
        
        return newImage;
    }
}

VkFilter ExtractFilter(const fastgltf::Filter filter)
{
    switch (filter)
    {
    // Nearest samplers
    case fastgltf::Filter::Nearest:
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::NearestMipMapLinear:
        return VK_FILTER_NEAREST;
    // Linear filters
    case fastgltf::Filter::Linear:
    case fastgltf::Filter::LinearMipMapNearest:
    case fastgltf::Filter::LinearMipMapLinear:
    default:
        return VK_FILTER_LINEAR;
    }
}

VkSamplerMipmapMode ExtractMipMapMode(
    const fastgltf::Filter filter
) {
    switch (filter) {
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::LinearMipMapNearest:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;

    case fastgltf::Filter::NearestMipMapLinear:
    case fastgltf::Filter::LinearMipMapLinear:
    default:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

void blackbox::LoadedGLTF::Draw(
    const mat4& topMatrix,
    DrawContext& ctx
) {
    // Create renderables from the scenenodes
    for (const auto& node : topNodes)
    {
        node->Draw(topMatrix, ctx);
    }
}

void blackbox::LoadedGLTF::ClearAll()
{
    const VkDevice device = creator->device;

    descriptorPool.DestroyPools(device);
    creator->DestroyBuffer(materialDataBuffer);

    for (const auto& v : meshes | std::views::values) {

        creator->DestroyBuffer(v->meshBuffers.indexBuffer);
        creator->DestroyBuffer(v->meshBuffers.vertexBuffer);
    }

    for (auto& v : images | std::views::values) {
        
        if (v.image == creator->checkerboardImage.image) {
            // Dont destroy the default images
            continue;
        }
        creator->DestroyImage(v);
    }

    for (const auto& sampler : samplers) {
        vkDestroySampler(device, sampler, nullptr);
    }
}

std::optional<std::shared_ptr<blackbox::LoadedGLTF>> blackbox::LoadGLTF(VulkanRenderer* renderer, const std::filesystem::path& filePath)
{
    LogRenderer->Trace("Loading GLTF {}.", filePath.string());

    auto scene = std::make_shared<LoadedGLTF>();
    scene->creator = renderer;
    LoadedGLTF& file = *scene.get();

    fastgltf::Parser parser {};
    constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble | fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages;

    auto gltfFile = fastgltf::GltfDataBuffer::FromPath(filePath);
    if (!static_cast<bool>(gltfFile))
    {
        LogRenderer->Error("Failed to load GLTF file: {}. Reason: {}.", filePath.string(), fastgltf::getErrorMessage(gltfFile.error()));
    }
    fastgltf::Asset gltf {};
    std::filesystem::path path = filePath;
    
    auto type = fastgltf::determineGltfFileType(gltfFile.get());
    if (type == fastgltf::GltfType::glTF)
    {
        auto asset = parser.loadGltf(gltfFile.get(), path.parent_path(), gltfOptions);
        if (asset.error() != fastgltf::Error::None)
        {
            LogRenderer->Error("Failed to load GLTF file: {}. Reason: {}.", filePath.string(), fastgltf::getErrorMessage(asset.error()));
            return {};
        }

        gltf = std::move(asset.get());
    }
    else if (type == fastgltf::GltfType::GLB)
    {
        auto asset = parser.loadGltfBinary(gltfFile.get(), path.parent_path(), gltfOptions);
        if (asset.error() != fastgltf::Error::None)
        {
            LogRenderer->Error("Failed to load GLTF file: {}. Reason: {}.", filePath.string(), fastgltf::getErrorMessage(asset.error()));
            return {};
        }

        gltf = std::move(asset.get());
    }
    else
    {
        LogRenderer->Error("Failed to determine GLTF conainer.");
        return {};
    }

    std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes
    {
        {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .ratio = 3},
        {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .ratio = 3},
        {.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .ratio = 1},
    };
    file.descriptorPool.Init(renderer->device, (uint32_t)gltf.materials.size(), sizes);

    // Load samplers
    for (fastgltf::Sampler& sampler : gltf.samplers)
    {
        VkSamplerCreateInfo samplerInfo
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = ExtractFilter(sampler.magFilter.value_or(fastgltf::Filter::Nearest)),
            .minFilter = ExtractFilter(sampler.minFilter.value_or(fastgltf::Filter::Nearest)),
            .mipmapMode = ExtractMipMapMode(sampler.minFilter.value_or(fastgltf::Filter::Nearest)),
            .minLod = 0,
            .maxLod = VK_LOD_CLAMP_NONE,
        };

        VkSampler newSampler {};
        vkCreateSampler(renderer->device, &samplerInfo, nullptr, &newSampler);

        file.samplers.push_back(newSampler);
    }

    // Temporal arrays for all the objects to use while creating the GLTF data
    std::vector<std::shared_ptr<MeshAsset>> meshes {};
    std::vector<std::shared_ptr<Node>> nodes {};
    std::vector<AllocatedImage> images {};
    std::vector<std::shared_ptr<GLTFMaterial>> materials {};

    // Load all textures
    for (fastgltf::Image& image : gltf.images)
    {        
        std::optional<AllocatedImage> img = LoadGLTFImage(renderer, gltf, image);
        if (img.has_value())
        {
            images.push_back(*img);
            file.images[image.name.c_str()] = *img;
        }
        else
        {
            // We failed to load, so let's give the slot a default texture to not completely break loading
            images.push_back(renderer->checkerboardImage);
            LogRenderer->Warn("GLTF failed to load texture {}", image.name);
        }
    }

    // Create the buffer to hold the material data
    file.materialDataBuffer = renderer->CreateBuffer(sizeof(GLTFMetallicRoughness::MaterialConstants) * gltf.materials.size(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    int dataIndex = 0;
    GLTFMetallicRoughness::MaterialConstants* sceneMaterialConstants = (GLTFMetallicRoughness::MaterialConstants*)file.materialDataBuffer.info.pMappedData;

    for (fastgltf::Material& material : gltf.materials)
    {
        std::shared_ptr<GLTFMaterial> newMaterial = std::make_shared<GLTFMaterial>();
        materials.push_back(newMaterial);
        file.materials[material.name.c_str()] = newMaterial;

        GLTFMetallicRoughness::MaterialConstants constants
        {
            .colorFactors
            {
                material.pbrData.baseColorFactor[0],
                material.pbrData.baseColorFactor[1],
                material.pbrData.baseColorFactor[2],
                material.pbrData.baseColorFactor[3],
            },
            .metallicRoughnessFactors
            {
                material.pbrData.metallicFactor,
                material.pbrData.roughnessFactor,
                0.0f,
                0.0f,
            },
        };

        // Write material parameters to the buffer
        sceneMaterialConstants[dataIndex] = constants;

        MaterialPass passType = MaterialPass::MainColor;
        if (material.alphaMode == fastgltf::AlphaMode::Blend)
        {
            passType = MaterialPass::Transparent;
        }

        GLTFMetallicRoughness::MaterialResources materialResources
        {
            // Set default material textures
            .colorImage = renderer->whiteImage,
            .colorSampler = renderer->defaultSamplerLinear,
            .metallicRoughnessImage = renderer->whiteImage,
            .metallicRoughnessSampler = renderer->defaultSamplerLinear,

            // Set the uniform buffer for the material data
            .dataBuffer = file.materialDataBuffer.buffer,
            .dataBufferOffset = (uint32_t)(dataIndex * sizeof(GLTFMetallicRoughness::MaterialConstants)),
        };

        // Grab textures for the material data
        if (material.pbrData.baseColorTexture.has_value())
        {
            size_t image = gltf.textures[material.pbrData.baseColorTexture.value().textureIndex].imageIndex.value();
            size_t sampler = gltf.textures[material.pbrData.baseColorTexture.value().textureIndex].samplerIndex.value();

            materialResources.colorImage = images[image];
            materialResources.colorSampler = file.samplers[sampler];
        }

        // Build material
        newMaterial->data = renderer->metallicRoughnessMaterial.WriteMaterial(renderer->device, passType, materialResources, file.descriptorPool);

        dataIndex++;
    }

    // Use the same vectors for all meshes so that the memory doesn't reallocate as often
    std::vector<uint32_t> indices {};
    std::vector<Vertex> vertices {};

    for (fastgltf::Mesh& mesh : gltf.meshes)
    {
        std::shared_ptr<MeshAsset> newMesh = std::make_shared<MeshAsset>();
        meshes.push_back(newMesh);

        file.meshes[mesh.name.c_str()] = newMesh;
        newMesh->name = mesh.name;

        // Clear the mesh arrays each mesh. We don't want to merge them by accident
        indices.clear();
        vertices.clear();

        for (auto&& primitive : mesh.primitives)
        {
            GeoSurface surface
            {
                .startIndex = (uint32_t)indices.size(),
                .count = (uint32_t)gltf.accessors[primitive.indicesAccessor.value()].count,
            };

            size_t initialVertex = vertices.size();

            // Load indices
            {
                fastgltf::Accessor& indexAccessor = gltf.accessors[primitive.indicesAccessor.value()];
                indices.reserve(indices.size() + indexAccessor.count);

                fastgltf::iterateAccessor<std::uint32_t>(gltf, indexAccessor, [&](const std::uint32_t index)
                {
                    indices.push_back(static_cast<uint32_t>(index + initialVertex));
                });
            }

            // Load vertex positions
            {
                fastgltf::Accessor& posAccessor = gltf.accessors[primitive.findAttribute("POSITION")->accessorIndex];
                vertices.resize(vertices.size() + posAccessor.count);

                fastgltf::iterateAccessorWithIndex<float3>(gltf, posAccessor, [&](float3 v, size_t index)
                {
                    const Vertex vertex
                    {
                        .position = v,
                        .uvX = 0,
                        .normal = { 1, 0, 0 },
                        .uvY = 0,
                        .color = float4 { 1.f },
                    };
                    vertices[initialVertex + index] = vertex;
                });
            }

            // Load vertex normals
            auto normals = primitive.findAttribute("NORMAL");
            if (normals != primitive.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<float3>(gltf, gltf.accessors[normals->accessorIndex], [&](float3 v, size_t index)
                {
                    vertices[initialVertex + index].normal = v;
                });
            }

            // load UVs
            auto uv = primitive.findAttribute("TEXCOORD_0");
            if (uv != primitive.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<float2>(gltf, gltf.accessors[uv->accessorIndex], [&](float2 v, size_t index)
                {
                    vertices[initialVertex + index].uvX = v.x;
                    vertices[initialVertex + index].uvY = v.y;
                });
            }

            // Load vertex colors
            auto colors = primitive.findAttribute("COLOR_0");
            if (colors != primitive.attributes.end()) {

                fastgltf::iterateAccessorWithIndex<float4>(gltf, gltf.accessors[colors->accessorIndex], [&](float4 v, size_t index)
                {
                    vertices[initialVertex + index].color = v;
                });
            }

            if (primitive.materialIndex.has_value()) {
                surface.material = materials[primitive.materialIndex.value()];
            } else {
                surface.material = materials[0];
            }

            newMesh->surfaces.push_back(surface);
        }

        newMesh->meshBuffers = renderer->UploadMesh(indices, vertices);
    }

    // Load all nodes and their meshes
    for (fastgltf::Node& node : gltf.nodes)
    {
        std::shared_ptr<Node> newNode {};

        // Find if the node has a mesh, and if it does, hook it to the mesh pointer and allocate it with the meshnode class
        if (node.meshIndex.has_value())
        {
            newNode = std::make_shared<MeshNode>();
            static_cast<MeshNode*>(newNode.get())->mesh = meshes[*node.meshIndex];
        }
        else
        {
            newNode = std::make_shared<Node>();
        }

        nodes.push_back(newNode);
        file.nodes[node.name.c_str()];

        std::visit(fastgltf::visitor
        {
            [&](fastgltf::math::fmat4x4 matrix)
            {
                memcpy(&newNode->localTransform, matrix.data(), sizeof(matrix));
            },
            [&](fastgltf::TRS transform)
            {
                float3 translation(transform.translation[0], transform.translation[1], transform.translation[2]);
                quat rotation(transform.rotation[3], transform.rotation[0], transform.rotation[1], transform.rotation[2]);
                float3 scale(transform.scale[0], transform.scale[1], transform.scale[2]);

                mat4 transformMatrix = glm::translate(mat4(1.0f), translation);
                mat4 rotationMatrix = glm::mat4_cast(rotation);
                mat4 scaleMatrix = glm::scale(mat4(1.0f), scale);

                newNode->localTransform = transformMatrix * rotationMatrix * scaleMatrix;
            },
        }, node.transform);
    }

    // Run loop again to set up the transform hierarchy
    for (int i = 0; i < gltf.nodes.size(); i++)
    {
        fastgltf::Node& node = gltf.nodes[i];
        std::shared_ptr<Node>& sceneNode = nodes[i];

        for (size_t& child : node.children)
        {
            sceneNode->children.push_back(nodes[child]);
            nodes[child]->parent = sceneNode;
        }
    }

    // Find the top nodes (with no parents)
    for (auto& node : nodes)
    {
        if (node->parent.lock() == nullptr)
        {
            file.topNodes.push_back(node);
            node->RefreshTransform(mat4{1.0f});
        }
    }

    return scene;
}
