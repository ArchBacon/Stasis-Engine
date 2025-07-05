#include "vk_pipelines.h"
#include <fstream>
#include "vk_initializers.h"

bool vkutil::LoadShaderModule(
    const char* FilePath,
    VkDevice Device,
    VkShaderModule* OutShaderModule
) {
    // Open the file, with the cursor at the end
    std::ifstream File(FilePath, std::ios::ate | std::ios::binary);
    if (!File.is_open())
    {
        return false;
    }

    // Find what the size of the file is by looking up the location of the cursor.
    // Because the cursor is at the end, it gives the size directly in bytes.
    size_t FileSize = File.tellg();

    // SpirV expects the buffer to be on uint32, so make sure to reserve an int vector big enough for the entire file
    std::vector<uint32_t> Buffer(FileSize / sizeof(uint32_t));

    // Put the file cursor at the beginning
    File.seekg(0);

    // Load the entire file into the buffer
    File.read(reinterpret_cast<char*>(Buffer.data()), FileSize);

    // Now that the file is loaded into the buffer, we can close it.
    File.close();

    // Create a new shader module using the buffer we loaded
    VkShaderModuleCreateInfo CreateInfo {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    CreateInfo.pNext = nullptr;

    // [codeSize] has to be in bytes, so multiply the ints in the buffer by the side of the int to know the real size of the buffer
    CreateInfo.codeSize = Buffer.size() * sizeof(uint32_t);
    CreateInfo.pCode = Buffer.data();

    // Check that the creation goes well
    VkShaderModule ShaderModule {};
    if (vkCreateShaderModule(Device, &CreateInfo, nullptr, &ShaderModule) != VK_SUCCESS)
    {
        return false;
    }

    *OutShaderModule = ShaderModule;
    
    return true;
}
