#pragma once

#include <vulkan/vulkan.h>

namespace vkutil {
    void TransitionImage(VkCommandBuffer Command, VkImage Image, VkImageLayout CurrentLayout, VkImageLayout NewLayout);
    void CopyImageToImage(VkCommandBuffer Command, VkImage Source, VkImage Destination, VkExtent2D SourceSize, VkExtent2D DestSize);
};