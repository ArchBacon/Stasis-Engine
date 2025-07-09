#include "vk_initializers.h"

VkCommandPoolCreateInfo Blackbox::vkinit::CommandPoolCreateInfo(
    const uint32_t queueFamilyIndex,
    const VkCommandPoolCreateFlags flags
) {
    return VkCommandPoolCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = flags,
        .queueFamilyIndex = queueFamilyIndex,
    };
}

VkCommandBufferAllocateInfo Blackbox::vkinit::CommandBufferAllocateInfo(
    const VkCommandPool pool,
    const uint32_t count
) {
    return VkCommandBufferAllocateInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = count,
    };
}

VkCommandBufferBeginInfo Blackbox::vkinit::CommandBufferBeginInfo(
    const VkCommandBufferUsageFlags flags
) {
    return VkCommandBufferBeginInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = flags,
    };
}

VkFenceCreateInfo Blackbox::vkinit::FenceCreateInfo(
    const VkFenceCreateFlags flags
) {
    return VkFenceCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = flags,
    };
}

VkSemaphoreCreateInfo Blackbox::vkinit::SemaphoreCreateInfo(
    const VkSemaphoreCreateFlags flags
) {
    return VkSemaphoreCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .flags = flags,
    };
}

VkSemaphoreSubmitInfo Blackbox::vkinit::SemaphoreSubmitInfo(
    const VkPipelineStageFlags2 stageMask,
    const VkSemaphore semaphore
) {
	return VkSemaphoreSubmitInfo
    {
	    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = semaphore,
        .value = 1,
	    .stageMask = stageMask,
        .deviceIndex = 0,
    };
}

VkCommandBufferSubmitInfo Blackbox::vkinit::CommandBufferSubmitInfo(
    const VkCommandBuffer commandBuffer
) {
	return VkCommandBufferSubmitInfo
    {
	    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
	    .commandBuffer = commandBuffer,
	    .deviceMask = 0,
    };
}

VkSubmitInfo2 Blackbox::vkinit::SubmitInfo(
    const VkCommandBufferSubmitInfo* commandBuffer,
    const VkSemaphoreSubmitInfo* signalSemaphoreInfo,
    const VkSemaphoreSubmitInfo* waitSemaphoreInfo
) {
    return VkSubmitInfo2
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0u : 1u,
        .pWaitSemaphoreInfos = waitSemaphoreInfo,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = commandBuffer,
        .signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0u : 1u,
        .pSignalSemaphoreInfos = signalSemaphoreInfo,
    };
}

VkPresentInfoKHR Blackbox::vkinit::PresentInfo()
{
    return VkPresentInfoKHR
    {
        .sType =  VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 0,
        .swapchainCount = 0,
    };
}

VkRenderingAttachmentInfo Blackbox::vkinit::AttachmentInfo(
    const VkImageView view,
    const VkClearValue* clear,
    const VkImageLayout layout
) {
    return VkRenderingAttachmentInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = view,
        .imageLayout = layout,
        .loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = clear != nullptr ? *clear : VkClearValue{}
    };
}

VkRenderingAttachmentInfo Blackbox::vkinit::DepthAttachmentInfo(
    const VkImageView view,
    const VkImageLayout layout
) {
    return VkRenderingAttachmentInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = view,
        .imageLayout = layout,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue {.depthStencil {.depth = 0.f}},
    };
}

VkRenderingInfo Blackbox::vkinit::RenderingInfo(
    const VkExtent2D renderExtent,
    const VkRenderingAttachmentInfo* colorAttachment,
    const VkRenderingAttachmentInfo* depthAttachment
) {
    return VkRenderingInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = VkRect2D {VkOffset2D {0, 0}, renderExtent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = colorAttachment,
        .pDepthAttachment = depthAttachment,
    };
}

VkImageSubresourceRange Blackbox::vkinit::ImageSubresourceRange(
    const VkImageAspectFlags aspectMask
) {
    return VkImageSubresourceRange
    {
        .aspectMask = aspectMask,
        .baseMipLevel = 0,
        .levelCount = VK_REMAINING_MIP_LEVELS,
        .baseArrayLayer = 0,
        .layerCount = VK_REMAINING_ARRAY_LAYERS,
    };
}

VkDescriptorSetLayoutBinding Blackbox::vkinit::DescriptorsetLayoutBinding(
    const VkDescriptorType type,
    const VkShaderStageFlags stageFlags,
    const uint32_t binding
) {
    return VkDescriptorSetLayoutBinding
    {
        .binding = binding,
        .descriptorType = type,
        .descriptorCount = 1,
        .stageFlags = stageFlags,
    };
}

VkDescriptorSetLayoutCreateInfo Blackbox::vkinit::DescriptorsetLayoutCreateInfo(
    const VkDescriptorSetLayoutBinding* bindings,
    const uint32_t bindingCount
) {
    return VkDescriptorSetLayoutCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .flags = 0,
        .bindingCount = bindingCount,
        .pBindings = bindings,
    };
}

VkWriteDescriptorSet Blackbox::vkinit::WriteDescriptorImage(
    const VkDescriptorType type,
    const VkDescriptorSet dstSet,
    const VkDescriptorImageInfo* imageInfo,
    const uint32_t binding
) {
    return VkWriteDescriptorSet
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = dstSet,
        .dstBinding = binding,
        .descriptorCount = 1,
        .descriptorType = type,
        .pImageInfo = imageInfo,
    };
}

VkWriteDescriptorSet Blackbox::vkinit::WriteDescriptorBuffer(
    const VkDescriptorType type,
    const VkDescriptorSet dstSet,
    const VkDescriptorBufferInfo* bufferInfo,
    const uint32_t binding
) {
    return VkWriteDescriptorSet
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = dstSet,
        .dstBinding = binding,
        .descriptorCount = 1,
        .descriptorType = type,
        .pBufferInfo = bufferInfo,
    };
}

VkDescriptorBufferInfo Blackbox::vkinit::BufferInfo(
    const VkBuffer buffer,
    const VkDeviceSize offset,
    const VkDeviceSize range
) {
    return VkDescriptorBufferInfo
    {
        .buffer = buffer,
        .offset = offset,
        .range = range,
    };
}

VkImageCreateInfo Blackbox::vkinit::ImageCreateInfo(
    const VkFormat format,
    const VkImageUsageFlags usageFlags,
    const VkExtent3D extent
) {
    return VkImageCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = extent,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT, // For MSAA. We will not be using it by default, so default it to 1 sample per pixel.
        .tiling = VK_IMAGE_TILING_OPTIMAL, // Optimal tiling, which means the image is stored in the best gpu format
        .usage = usageFlags,
    };
}

VkImageViewCreateInfo Blackbox::vkinit::ImageviewCreateInfo(
    const VkFormat format,
    const VkImage image,
    const VkImageAspectFlags aspectFlags
) {
    // Build an image-view for the depth image to use for rendering
    return VkImageViewCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange{
            .aspectMask = aspectFlags,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };
}

VkPipelineLayoutCreateInfo Blackbox::vkinit::PipelineLayoutCreateInfo()
{
    return VkPipelineLayoutCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .flags = 0,
        .setLayoutCount = 0,
        .pushConstantRangeCount = 0,
    };
}

VkPipelineShaderStageCreateInfo Blackbox::vkinit::PipelineShaderStageCreateInfo(
    const VkShaderStageFlagBits stage,
    const VkShaderModule shaderModule,
    const char* entry
) {
    return VkPipelineShaderStageCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = stage, // Shader stage
        .module = shaderModule, // Module containing the code for this shader stage
        .pName = entry, // the entry point of the shader
    };
}
