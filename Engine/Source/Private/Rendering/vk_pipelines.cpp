#include "vk_pipelines.h"

#include <filesystem>
#include <fstream>
#include "vk_initializers.h"

bool Stasis::vkutil::LoadShaderModule(
    const char* filePath,
    const VkDevice device,
    VkShaderModule* outShaderModule
) {
    // Open the file, with the cursor at the end
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        const std::filesystem::path absoluteFilePath = std::filesystem::absolute(filePath);
        LogRenderer->Error("Failed to open shader file: {}", absoluteFilePath.string());
        return false;
    }

    // Find what the size of the file is by looking up the location of the cursor.
    // Because the cursor is at the end, it gives the size directly in bytes.
    const size_t fileSize = file.tellg();

    // SpirV expects the buffer to be on uint32, so make sure to reserve an int vector big enough for the entire file
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    // Put the file cursor at the beginning
    file.seekg(0);

    // Load the entire file into the buffer
    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(fileSize));

    // Now that the file is loaded into the buffer, we can close it.
    file.close();

    // Create a new shader module using the buffer we loaded
    const VkShaderModuleCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = buffer.size() * sizeof(uint32_t),
        .pCode = buffer.data(),
    };

    // Check that the creation goes well
    VkShaderModule shaderModule {};
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        return false;
    }

    *outShaderModule = shaderModule;
    
    return true;
}
