#pragma once

#include "vk_types.h"

namespace Blackbox::vkutil {
    bool LoadShaderModule(const char* filePath, VkDevice device, VkShaderModule* outShaderModule);
};
