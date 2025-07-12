#include "vk_pipelines.h"

#include <filesystem>
#include <fstream>
#include "vk_initializers.h"

bool blackbox::vkutil::LoadShaderModule(
    const char* filePath,
    const VkDevice device,
    VkShaderModule* outShaderModule
) {
    std::string compiledFilePath = std::string(filePath) + ".spv";
    
    // Open the file, with the cursor at the end
    std::ifstream file(compiledFilePath, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        const std::filesystem::path absoluteFilePath = std::filesystem::absolute(compiledFilePath);
        LogRenderer->Error("Failed to open shader file: {}", absoluteFilePath.string());
        return false;
    }

    // Find what the size of the file is by looking up the location of the cursor.
    // Because the cursor is at the end, it gives the size directly in bytes.
    const size_t fileSize = file.tellg();

    // SpirV expects the buffer to be on uint32, so make sure to reserve an int vector big enough for the entire file
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    // Put the file cursor at the beginning
    file.seekg(0);

    // Load the entire file into the buffer
    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(fileSize));

    // Now that the file is loaded into the buffer, we can close it.
    file.close();

    // Create a new shader module using the buffer we loaded
    const VkShaderModuleCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = buffer.size() * sizeof(uint32_t),
        .pCode = buffer.data(),
    };

    // Check that the creation goes well
    VkShaderModule shaderModule {};
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        return false;
    }

    *outShaderModule = shaderModule;
    
    return true;
}

void blackbox::PipelineBuilder::Clear()
{
    inputAssembly = {.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    rasterizer = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    colorBlendAttachment = {};
    multisampling = {.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    pipelineLayout = {};
    depthStencil = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    renderInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
    shaderStages.clear();
}

VkPipeline blackbox::PipelineBuilder::Build(
    VkDevice device
) {
    // MAke the viewport state from our stored viewport and scissor.
    // At the moment, we wont support multiple viewports or scissorts.
    VkPipelineViewportStateCreateInfo viewportState
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    // Setup dummy color blending. We aren't using transparent objects yet.
    // The blending is just "no blend", but we do write to the color attachment
    VkPipelineColorBlendStateCreateInfo colorBlending
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
    };

    // Completely clear VertexInputStateCreateInfo, as we have no need for it.
    VkPipelineVertexInputStateCreateInfo vertexInputState {.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

    // Build the actual pipeline
    // We will now use all the info structs we have been writing into this one to create the pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        // Connect the renderInfo to the pNext extension mechanism
        .pNext = &renderInfo,
        
        .stageCount = static_cast<uint32_t>(shaderStages.size()),
        .pStages = shaderStages.data(),
        .pVertexInputState = &vertexInputState,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depthStencil,
        .pColorBlendState = &colorBlending,
        .layout = pipelineLayout,
    };

    VkDynamicState state[]
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamicState
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates = &state[0],
    };
    
    pipelineInfo.pDynamicState = &dynamicState;

    // It's easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
    VkPipeline pipeline {};
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
    {
        LogRenderer->Error("Failed to create graphics pipeline");
        return VK_NULL_HANDLE;
    }

    return pipeline;
}

void blackbox::PipelineBuilder::SetShaders(
    VkShaderModule vertexShader,
    VkShaderModule fragmentShader
) {
    shaderStages.clear();
    shaderStages.push_back(vkinit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader));
    shaderStages.push_back(vkinit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader));
}

void blackbox::PipelineBuilder::SetInputTopology(
    VkPrimitiveTopology topology
) {
    inputAssembly.topology = topology;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void blackbox::PipelineBuilder::SetPolygonMode(
    VkPolygonMode mode
) {
    rasterizer.polygonMode = mode;
    rasterizer.lineWidth = 1.0f;
}

void blackbox::PipelineBuilder::SetCullMode(
    VkCullModeFlags mode,
    VkFrontFace frontFace
) {
    rasterizer.cullMode = mode;
    rasterizer.frontFace = frontFace;
}

void blackbox::PipelineBuilder::SetMultisamplingNone()
{
    multisampling.sampleShadingEnable = VK_FALSE;
    // Multisampling defaulted to no multisampling (1 sample per pixel)
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    // No alpha to covarage either
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;
}

void blackbox::PipelineBuilder::DisableBlending()
{
    // Default write mask
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // No blending
    colorBlendAttachment.blendEnable = VK_FALSE;
}

void blackbox::PipelineBuilder::SetColorAttachmentFormat(
    VkFormat format
) {
    colorAttachmentFormat = format;
    renderInfo.colorAttachmentCount = 1;
    renderInfo.pColorAttachmentFormats = &colorAttachmentFormat;
}

void blackbox::PipelineBuilder::SetDepthFormat(
    VkFormat format
) {
    renderInfo.depthAttachmentFormat = format;
}

void blackbox::PipelineBuilder::DisableDepthTest()
{
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.front = {};
    depthStencil.back = {};
    depthStencil.minDepthBounds = 0.0f;
    depthStencil.maxDepthBounds = 1.0f;
}

void blackbox::PipelineBuilder::EnableDepthTest(
    const bool depthWriteEnable,
    const VkCompareOp op
) {
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = depthWriteEnable;
    depthStencil.depthCompareOp = op;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {};
    depthStencil.back = {};
    depthStencil.minDepthBounds = 0.f;
    depthStencil.maxDepthBounds = 1.f;
}
