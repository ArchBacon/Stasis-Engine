#include "vk_images.h"
#include "vk_initializers.h"

void blackbox::vkutil::TransitionImage(
    const VkCommandBuffer command,
    const VkImage image,
    const VkImageLayout currentLayout,
    const VkImageLayout newLayout
) {
    VkImageMemoryBarrier2 imageBarrier { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
    imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
    
    imageBarrier.oldLayout = currentLayout;
    imageBarrier.newLayout = newLayout;

    const VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier.subresourceRange = vkinit::ImageSubresourceRange(aspectMask);
    imageBarrier.image = image;

    VkDependencyInfo dependencyInfo {};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &imageBarrier;

    vkCmdPipelineBarrier2(command, &dependencyInfo);
}

void blackbox::vkutil::CopyImageToImage(
    const VkCommandBuffer command,
    const VkImage source,
    const VkImage destination,
    const VkExtent2D srcSize,
    const VkExtent2D dstSize
) {
    VkImageBlit2 blitRegion {};
    blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
    
    blitRegion.srcOffsets[1].x = static_cast<int32_t>(srcSize.width);
    blitRegion.srcOffsets[1].y = static_cast<int32_t>(srcSize.height);
    blitRegion.srcOffsets[1].z = 1;

    blitRegion.dstOffsets[1].x = static_cast<int32_t>(dstSize.width);
    blitRegion.dstOffsets[1].y = static_cast<int32_t>(dstSize.height);
    blitRegion.dstOffsets[1].z = 1;
    
    blitRegion.srcSubresource = { .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1};
    blitRegion.dstSubresource = { .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1};

    const VkBlitImageInfo2 blitInfo
    {
        .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
        .srcImage = source,
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstImage = destination,
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .regionCount = 1,
        .pRegions = &blitRegion,
        .filter = VK_FILTER_LINEAR,
    };

    vkCmdBlitImage2(command, &blitInfo);
}
