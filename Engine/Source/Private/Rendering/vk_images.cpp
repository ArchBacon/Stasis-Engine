#include "vk_images.h"
#include "vk_initializers.h"

void vkutil::TransitionImage(
    const VkCommandBuffer Command,
    const VkImage Image,
    const VkImageLayout CurrentLayout,
    const VkImageLayout NewLayout
) {
    VkImageMemoryBarrier2 ImageBarrier { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
    ImageBarrier.pNext = nullptr;

    ImageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    ImageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    ImageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    ImageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
    
    ImageBarrier.oldLayout = CurrentLayout;
    ImageBarrier.newLayout = NewLayout;

    const VkImageAspectFlags AspectMask = (NewLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    ImageBarrier.subresourceRange = vkinit::ImageSubresourceRange(AspectMask);
    ImageBarrier.image = Image;

    VkDependencyInfo DependencyInfo {};
    DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    DependencyInfo.pNext = nullptr;

    DependencyInfo.imageMemoryBarrierCount = 1;
    DependencyInfo.pImageMemoryBarriers = &ImageBarrier;

    vkCmdPipelineBarrier2(Command, &DependencyInfo);
}

void vkutil::CopyImageToImage(
    const VkCommandBuffer Command,
    const VkImage Source,
    const VkImage Destination,
    const VkExtent2D SourceSize,
    const VkExtent2D DestSize
) {
    VkImageBlit2 BlitRegion { .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

    BlitRegion.srcOffsets[1].x = SourceSize.width;
    BlitRegion.srcOffsets[1].y = SourceSize.height;
    BlitRegion.srcOffsets[1].z = 1;

    BlitRegion.dstOffsets[1].x = DestSize.width;
    BlitRegion.dstOffsets[1].y = DestSize.height;
    BlitRegion.dstOffsets[1].z = 1;

    BlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    BlitRegion.srcSubresource.baseArrayLayer = 0;
    BlitRegion.srcSubresource.layerCount = 1;
    BlitRegion.srcSubresource.mipLevel = 0;

    BlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    BlitRegion.dstSubresource.baseArrayLayer = 0;
    BlitRegion.dstSubresource.layerCount = 1;
    BlitRegion.dstSubresource.mipLevel = 0;

    VkBlitImageInfo2 BlitInfo { .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
    BlitInfo.dstImage = Destination;
    BlitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    BlitInfo.srcImage = Source;
    BlitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    BlitInfo.filter = VK_FILTER_LINEAR;
    BlitInfo.regionCount = 1;
    BlitInfo.pRegions = &BlitRegion;

    vkCmdBlitImage2(Command, &BlitInfo);
}
