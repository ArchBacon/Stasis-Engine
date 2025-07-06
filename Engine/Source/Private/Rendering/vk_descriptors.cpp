#include "vk_descriptors.h"

void Stasis::DescriptorLayoutBuilder::AddBinding(
    const uint32_t binding,
    const VkDescriptorType type
) {
    const VkDescriptorSetLayoutBinding newBinding
    {
        .binding = binding,
        .descriptorType = type,
        .descriptorCount = 1,
    };
    
    bindings.push_back(newBinding);   
}

void Stasis::DescriptorLayoutBuilder::Clear()
{
    bindings.clear();
}

VkDescriptorSetLayout Stasis::DescriptorLayoutBuilder::Build(
    const VkDevice device,
    const VkShaderStageFlags shaderStages,
    const void* pNext,
    const VkDescriptorSetLayoutCreateFlags flags
) {
    for (auto& binding : bindings)
    {
        binding.stageFlags |= shaderStages;
    }

    const VkDescriptorSetLayoutCreateInfo layoutInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = pNext,
        .flags = flags,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data(),
    };

    VkDescriptorSetLayout set {};
    VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &set));

    return set;
}

void Stasis::DescriptorAllocator::InitPool(
    const VkDevice device,
    const uint32_t maxSets,
    const std::span<PoolSizeRatio> poolRatios
) {
    std::vector<VkDescriptorPoolSize> poolSizes {};
    for (const auto [Type, Ratio] : poolRatios)
    {
        poolSizes.push_back(VkDescriptorPoolSize {
            .type = Type,
            .descriptorCount = static_cast<uint32_t>(Ratio * static_cast<float>(maxSets))
        });
    }

    const VkDescriptorPoolCreateInfo poolInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = 0,
        .maxSets = maxSets,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data(),
    };

    vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);
}

void Stasis::DescriptorAllocator::ClearDescriptors(
    const VkDevice device
) {
    vkResetDescriptorPool(device, pool, 0);
}

void Stasis::DescriptorAllocator::DestroyPool(
    const VkDevice device
) {
    vkDestroyDescriptorPool(device, pool, nullptr);
}

VkDescriptorSet Stasis::DescriptorAllocator::Allocate(
    const VkDevice device,
    const VkDescriptorSetLayout layout
) {
    const VkDescriptorSetAllocateInfo allocInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &layout,
    };

    VkDescriptorSet descriptorSet {};
    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet));

    return descriptorSet;
}
