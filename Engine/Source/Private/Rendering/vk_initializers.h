#pragma once

#include "vk_types.h"

namespace vkinit
{
    //> init_cmd
    VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t QueueFamilyIndex, VkCommandPoolCreateFlags Flags = 0);
    VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool Pool, uint32_t Count = 1);
    //< init_cmd

    VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags Flags = 0);
    VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer Cmd);

    VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags Flags = 0);
    VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags Flags = 0);

    VkSubmitInfo2 SubmitInfo(const VkCommandBufferSubmitInfo* Cmd, const VkSemaphoreSubmitInfo* SignalSemaphoreInfo, const VkSemaphoreSubmitInfo* WaitSemaphoreInfo);
    VkPresentInfoKHR PresentInfo();
    
    VkRenderingAttachmentInfo AttachmentInfo(VkImageView View, const VkClearValue* Clear, VkImageLayout Layout /*= VKIMAGELAYOUTCOLORATTACHMENTOPTIMAL*/);
    VkRenderingAttachmentInfo DepthAttachmentInfo(VkImageView View, VkImageLayout Layout /*= VKIMAGELAYOUTCOLORATTACHMENTOPTIMAL*/);
    VkRenderingInfo RenderingInfo(VkExtent2D RenderExtent, const VkRenderingAttachmentInfo* ColorAttachment, const VkRenderingAttachmentInfo* DepthAttachment);
    VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags AspectMask);

    VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 StageMask, VkSemaphore Semaphore);
    VkDescriptorSetLayoutBinding DescriptorsetLayoutBinding(VkDescriptorType Type, VkShaderStageFlags StageFlags, uint32_t Binding);
    VkDescriptorSetLayoutCreateInfo DescriptorsetLayoutCreateInfo(const VkDescriptorSetLayoutBinding* Bindings, uint32_t BindingCount);
    VkWriteDescriptorSet WriteDescriptorImage(VkDescriptorType Type, VkDescriptorSet DstSet, const VkDescriptorImageInfo* ImageInfo, uint32_t Binding);
    VkWriteDescriptorSet WriteDescriptorBuffer(VkDescriptorType Type, VkDescriptorSet DstSet, const VkDescriptorBufferInfo* BufferInfo, uint32_t Binding);
    VkDescriptorBufferInfo BufferInfo(VkBuffer Buffer, VkDeviceSize Offset, VkDeviceSize Range);

    VkImageCreateInfo ImageCreateInfo(VkFormat Format, VkImageUsageFlags UsageFlags, VkExtent3D Extent);
    VkImageViewCreateInfo ImageviewCreateInfo(VkFormat Format, VkImage Image, VkImageAspectFlags AspectFlags);
    VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo();
    VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits Stage, VkShaderModule ShaderModule, const char* Entry = "main");
}
