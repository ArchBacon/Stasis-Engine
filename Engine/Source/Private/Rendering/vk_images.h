#pragma once

#include <vulkan/vulkan.h>

namespace blackbox::vkutil {
    void TransitionImage(VkCommandBuffer command, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
    void CopyImageToImage(VkCommandBuffer command, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);
};
