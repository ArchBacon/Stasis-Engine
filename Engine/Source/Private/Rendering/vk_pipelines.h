#pragma once

#include "vk_types.h"

namespace Blackbox::vkutil
{
    bool LoadShaderModule(const char* filePath, VkDevice device, VkShaderModule* outShaderModule);
}

namespace Blackbox
{
    class PipelineBuilder
    {
    public:
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages {};

        VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
        VkPipelineRasterizationStateCreateInfo rasterizer {};
        VkPipelineColorBlendAttachmentState colorBlendAttachment {};
        VkPipelineMultisampleStateCreateInfo multisampling {};
        VkPipelineLayout pipelineLayout {};
        VkPipelineDepthStencilStateCreateInfo depthStencil {};
        VkPipelineRenderingCreateInfo renderInfo {};
        VkFormat colorAttachmentFormat {};

        PipelineBuilder() { Clear(); }

        void Clear();
        VkPipeline Build(VkDevice device);
        void SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
        void SetInputTopology(VkPrimitiveTopology topology);
        void SetPolygonMode(VkPolygonMode mode);
        void SetCullMode(VkCullModeFlags mode, VkFrontFace frontFace);
        void SetMultisamplingNone();
        void DisableBlending();
        void SetColorAttachmentFormat(VkFormat format);
        void SetDepthFormat(VkFormat format);
        void DisableDepthTest();
    };
}
