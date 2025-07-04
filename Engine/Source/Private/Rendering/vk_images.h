#pragma once

#include <vulkan/vulkan.h>

namespace vkutil {
    void TransitionImage(VkCommandBuffer Command, VkImage Image, VkImageLayout CurrentLayout, VkImageLayout NewLayout);
};