// ReSharper disable CppInitializedValueIsAlwaysRewritten
#include "vk_initializers.h"

//> init_cmd
VkCommandPoolCreateInfo vkinit::CommandPoolCreateInfo(
    const uint32_t QueueFamilyIndex,
    const VkCommandPoolCreateFlags Flags /*= 0*/
) {
    VkCommandPoolCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    Info.pNext = nullptr;
    Info.queueFamilyIndex = QueueFamilyIndex;
    Info.flags = Flags;
    
    return Info;
}

VkCommandBufferAllocateInfo vkinit::CommandBufferAllocateInfo(
    const VkCommandPool Pool,
    const uint32_t Count /*= 1*/
) {
    VkCommandBufferAllocateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    Info.pNext = nullptr;

    Info.commandPool = Pool;
    Info.commandBufferCount = Count;
    Info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    
    return Info;
}
//< init_cmd

//> init_cmd_draw
VkCommandBufferBeginInfo vkinit::CommandBufferBeginInfo(
    const VkCommandBufferUsageFlags Flags /*= 0*/
) {
    VkCommandBufferBeginInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    Info.pNext = nullptr;
    Info.pInheritanceInfo = nullptr;
    Info.flags = Flags;
    
    return Info;
}
//< init_cmd_draw

//> init_sync
VkFenceCreateInfo vkinit::FenceCreateInfo(
    const VkFenceCreateFlags Flags /*= 0*/
) {
    VkFenceCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    Info.pNext = nullptr;
    Info.flags = Flags;

    return Info;
}

VkSemaphoreCreateInfo vkinit::SemaphoreCreateInfo(
    const VkSemaphoreCreateFlags Flags /*= 0*/
) {
    VkSemaphoreCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    Info.pNext = nullptr;
    Info.flags = Flags;
    
    return Info;
}
//< init_sync

//> init_submit
VkSemaphoreSubmitInfo vkinit::SemaphoreSubmitInfo(
    const VkPipelineStageFlags2 StageMask,
    const VkSemaphore Semaphore
) {
	VkSemaphoreSubmitInfo SubmitInfo{};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	SubmitInfo.pNext = nullptr;
	SubmitInfo.semaphore = Semaphore;
	SubmitInfo.stageMask = StageMask;
	SubmitInfo.deviceIndex = 0;
	SubmitInfo.value = 1;

	return SubmitInfo;
}

VkCommandBufferSubmitInfo vkinit::CommandBufferSubmitInfo(
    const VkCommandBuffer Cmd
) {
	VkCommandBufferSubmitInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	Info.pNext = nullptr;
	Info.commandBuffer = Cmd;
	Info.deviceMask = 0;

	return Info;
}

VkSubmitInfo2 vkinit::SubmitInfo(
    const VkCommandBufferSubmitInfo* Cmd,
    const VkSemaphoreSubmitInfo* SignalSemaphoreInfo,
    const VkSemaphoreSubmitInfo* WaitSemaphoreInfo
) {
    VkSubmitInfo2 Info = {};
    Info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    Info.pNext = nullptr;

    Info.waitSemaphoreInfoCount = WaitSemaphoreInfo == nullptr ? 0 : 1;
    Info.pWaitSemaphoreInfos = WaitSemaphoreInfo;

    Info.signalSemaphoreInfoCount = SignalSemaphoreInfo == nullptr ? 0 : 1;
    Info.pSignalSemaphoreInfos = SignalSemaphoreInfo;

    Info.commandBufferInfoCount = 1;
    Info.pCommandBufferInfos = Cmd;

    return Info;
}
//< init_submit

VkPresentInfoKHR vkinit::PresentInfo()
{
    VkPresentInfoKHR Info = {};
    Info.sType =  VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    Info.pNext = nullptr;

    Info.swapchainCount = 0;
    Info.pSwapchains = nullptr;
    Info.pWaitSemaphores = nullptr;
    Info.waitSemaphoreCount = 0;
    Info.pImageIndices = nullptr;

    return Info;
}

//> color_info
VkRenderingAttachmentInfo vkinit::AttachmentInfo(
    const VkImageView View,
    const VkClearValue* Clear,
    const VkImageLayout Layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/
) {
    VkRenderingAttachmentInfo ColorAttachment {};
    ColorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    ColorAttachment.pNext = nullptr;

    ColorAttachment.imageView = View;
    ColorAttachment.imageLayout = Layout;
    ColorAttachment.loadOp = Clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    if (Clear) {
        ColorAttachment.clearValue = *Clear;
    }

    return ColorAttachment;
}
//< color_info
//> depth_info
VkRenderingAttachmentInfo vkinit::DepthAttachmentInfo(
    const VkImageView View,
    const VkImageLayout Layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/
) {
    VkRenderingAttachmentInfo DepthAttachment {};
    DepthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    DepthAttachment.pNext = nullptr;

    DepthAttachment.imageView = View;
    DepthAttachment.imageLayout = Layout;
    DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    DepthAttachment.clearValue.depthStencil.depth = 0.f;

    return DepthAttachment;
}
//< depth_info
//> render_info
VkRenderingInfo vkinit::RenderingInfo(
    const VkExtent2D RenderExtent,
    const VkRenderingAttachmentInfo* ColorAttachment,
    const VkRenderingAttachmentInfo* DepthAttachment
) {
    VkRenderingInfo RenderInfo {};
    RenderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    RenderInfo.pNext = nullptr;

    RenderInfo.renderArea = VkRect2D { VkOffset2D { 0, 0 }, RenderExtent };
    RenderInfo.layerCount = 1;
    RenderInfo.colorAttachmentCount = 1;
    RenderInfo.pColorAttachments = ColorAttachment;
    RenderInfo.pDepthAttachment = DepthAttachment;
    RenderInfo.pStencilAttachment = nullptr;

    return RenderInfo;
}
//< render_info
//> subresource
VkImageSubresourceRange vkinit::ImageSubresourceRange(
    const VkImageAspectFlags AspectMask
) {
    VkImageSubresourceRange SubImage {};
    SubImage.aspectMask = AspectMask;
    SubImage.baseMipLevel = 0;
    SubImage.levelCount = VK_REMAINING_MIP_LEVELS;
    SubImage.baseArrayLayer = 0;
    SubImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

    return SubImage;
}
//< subresource

VkDescriptorSetLayoutBinding vkinit::DescriptorsetLayoutBinding(
    const VkDescriptorType Type,
    const VkShaderStageFlags StageFlags,
    const uint32_t Binding
) {
    VkDescriptorSetLayoutBinding Setbind = {};
    Setbind.binding = Binding;
    Setbind.descriptorCount = 1;
    Setbind.descriptorType = Type;
    Setbind.pImmutableSamplers = nullptr;
    Setbind.stageFlags = StageFlags;

    return Setbind;
}

VkDescriptorSetLayoutCreateInfo vkinit::DescriptorsetLayoutCreateInfo(
    const VkDescriptorSetLayoutBinding* Bindings,
    const uint32_t BindingCount
) {
    VkDescriptorSetLayoutCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    Info.pNext = nullptr;

    Info.pBindings = Bindings;
    Info.bindingCount = BindingCount;
    Info.flags = 0;

    return Info;
}

VkWriteDescriptorSet vkinit::WriteDescriptorImage(
    const VkDescriptorType Type,
    const VkDescriptorSet DstSet,
    const VkDescriptorImageInfo* ImageInfo,
    const uint32_t Binding
) {
    VkWriteDescriptorSet Write = {};
    Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    Write.pNext = nullptr;

    Write.dstBinding = Binding;
    Write.dstSet = DstSet;
    Write.descriptorCount = 1;
    Write.descriptorType = Type;
    Write.pImageInfo = ImageInfo;

    return Write;
}

VkWriteDescriptorSet vkinit::WriteDescriptorBuffer(
    const VkDescriptorType Type,
    const VkDescriptorSet DstSet,
    const VkDescriptorBufferInfo* BufferInfo,
    const uint32_t Binding
) {
    VkWriteDescriptorSet Write = {};
    Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    Write.pNext = nullptr;

    Write.dstBinding = Binding;
    Write.dstSet = DstSet;
    Write.descriptorCount = 1;
    Write.descriptorType = Type;
    Write.pBufferInfo = BufferInfo;

    return Write;
}

VkDescriptorBufferInfo vkinit::BufferInfo(
    const VkBuffer Buffer,
    const VkDeviceSize Offset,
    const VkDeviceSize Range
) {
    VkDescriptorBufferInfo BufferInfo {};
    BufferInfo.buffer = Buffer;
    BufferInfo.offset = Offset;
    BufferInfo.range = Range;
    
    return BufferInfo;
}

//> image_set
VkImageCreateInfo vkinit::ImageCreateInfo(
    const VkFormat Format,
    const VkImageUsageFlags UsageFlags,
    const VkExtent3D Extent
) {
    VkImageCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    Info.pNext = nullptr;

    Info.imageType = VK_IMAGE_TYPE_2D;

    Info.format = Format;
    Info.extent = Extent;

    Info.mipLevels = 1;
    Info.arrayLayers = 1;

    //for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
    Info.samples = VK_SAMPLE_COUNT_1_BIT;

    //optimal tiling, which means the image is stored on the best gpu format
    Info.tiling = VK_IMAGE_TILING_OPTIMAL;
    Info.usage = UsageFlags;

    return Info;
}

VkImageViewCreateInfo vkinit::ImageviewCreateInfo(
    const VkFormat Format,
    const VkImage Image,
    const VkImageAspectFlags AspectFlags
) {
    // build an image-view for the depth image to use for rendering
    VkImageViewCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    Info.pNext = nullptr;

    Info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    Info.image = Image;
    Info.format = Format;
    Info.subresourceRange.baseMipLevel = 0;
    Info.subresourceRange.levelCount = 1;
    Info.subresourceRange.baseArrayLayer = 0;
    Info.subresourceRange.layerCount = 1;
    Info.subresourceRange.aspectMask = AspectFlags;

    return Info;
}
//< image_set
VkPipelineLayoutCreateInfo vkinit::PipelineLayoutCreateInfo()
{
    VkPipelineLayoutCreateInfo Info {};
    Info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    Info.pNext = nullptr;

    // empty defaults
    Info.flags = 0;
    Info.setLayoutCount = 0;
    Info.pSetLayouts = nullptr;
    Info.pushConstantRangeCount = 0;
    Info.pPushConstantRanges = nullptr;
    
    return Info;
}

VkPipelineShaderStageCreateInfo vkinit::PipelineShaderStageCreateInfo(
    const VkShaderStageFlagBits Stage,
    const VkShaderModule ShaderModule,
    const char* Entry
) {
    VkPipelineShaderStageCreateInfo Info {};
    Info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    Info.pNext = nullptr;

    // shader stage
    Info.stage = Stage;
    // module containing the code for this shader stage
    Info.module = ShaderModule;
    // the entry point of the shader
    Info.pName = Entry;
    
    return Info;
}
