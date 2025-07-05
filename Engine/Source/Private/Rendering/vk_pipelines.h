#pragma once

#include "vk_types.h"

namespace vkutil {
    bool LoadShaderModule(const char* FilePath, VkDevice Device, VkShaderModule* OutShaderModule);
};