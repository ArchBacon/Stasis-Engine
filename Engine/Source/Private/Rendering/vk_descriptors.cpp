#include "vk_descriptors.h"

#include <algorithm>

void blackbox::DescriptorLayoutBuilder::AddBinding(
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

void blackbox::DescriptorLayoutBuilder::Clear()
{
    bindings.clear();
}

VkDescriptorSetLayout blackbox::DescriptorLayoutBuilder::Build(
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

void blackbox::DescriptorAllocator::InitPool(
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

void blackbox::DescriptorAllocator::ClearDescriptors(
    const VkDevice device
) {
    vkResetDescriptorPool(device, pool, 0);
}

void blackbox::DescriptorAllocator::DestroyPool(
    const VkDevice device
) {
    vkDestroyDescriptorPool(device, pool, nullptr);
}

VkDescriptorSet blackbox::DescriptorAllocator::Allocate(
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

void blackbox::DescriptorAllocatorGrowable::Init(
    VkDevice device,
    uint32_t maxSets,
    std::span<PoolSizeRatio> poolRatios
) {
    ratios.clear();
    for (auto ratio : poolRatios)
    {
        ratios.push_back(ratio);
    }

    const VkDescriptorPool pool = CreatePool(device, maxSets, ratios);
    setsPerPool = static_cast<uint32_t>(static_cast<float>(maxSets) * 1.5f); // Grow it next allocation
    readyPools.push_back(pool);
}
void blackbox::DescriptorAllocatorGrowable::ClearPools(
    const VkDevice device
) {
    for (const auto pool : readyPools)
    {
        vkResetDescriptorPool(device, pool, 0);
    }

    for (auto pool : fullPools)
    {
        vkResetDescriptorPool(device, pool, 0);
        readyPools.push_back(pool);
    }
    fullPools.clear();
}

void blackbox::DescriptorAllocatorGrowable::DestroyPools(
    const VkDevice device
) {
    for (const auto pool : readyPools)
    {
        vkDestroyDescriptorPool(device, pool, nullptr);
    }
    readyPools.clear();

    for (auto pool : fullPools)
    {
        vkDestroyDescriptorPool(device, pool, nullptr);
        readyPools.push_back(pool);
    }
    fullPools.clear();
}

VkDescriptorSet blackbox::DescriptorAllocatorGrowable::Allocate(
    VkDevice device,
    VkDescriptorSetLayout layout,
    void* pNext
) {
    // Get or create a pool to allocate from
    VkDescriptorPool pool = GetPool(device);

    VkDescriptorSetAllocateInfo allocInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = pNext,
        .descriptorPool = pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &layout,
    };
    VkDescriptorSet descriptorSet {};
    
    const VkResult result = vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
    // Allocation failed. Try again
    if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
    {
        fullPools.push_back(pool);
        pool = GetPool(device);
        allocInfo.descriptorPool = pool;

        VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet));
    }

    readyPools.push_back(pool);

    return descriptorSet;   
}

VkDescriptorPool blackbox::DescriptorAllocatorGrowable::GetPool(
    const VkDevice device
) {
    if (!readyPools.empty())
    {
        const VkDescriptorPool pool = readyPools.back();
        readyPools.pop_back();

        return pool;
    }

    const VkDescriptorPool pool = CreatePool(device, setsPerPool, ratios);
    setsPerPool = std::min<uint32_t>(static_cast<uint32_t>(static_cast<float>(setsPerPool) * 1.5f), 4092);

    return pool;
}

VkDescriptorPool blackbox::DescriptorAllocatorGrowable::CreatePool(
    const VkDevice device,
    const uint32_t setCount,
    const std::span<PoolSizeRatio> poolRatios
) {
    std::vector<VkDescriptorPoolSize> poolSizes {};
    for (const auto ratio : poolRatios)
    {
        poolSizes.push_back(VkDescriptorPoolSize
        {
            .type = ratio.type,
            .descriptorCount = static_cast<uint32_t>(ratio.ratio * setCount)
        });
    }

    const VkDescriptorPoolCreateInfo poolInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = 0,
        .maxSets = setCount,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data(),
    };

    VkDescriptorPool pool {};
    vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);

    return pool;
}

void blackbox::DescriptorWriter::WriteImage(
    int binding,
    VkImageView image,
    VkSampler sampler,
    VkImageLayout layout,
    VkDescriptorType type
) {
    VkDescriptorImageInfo& imageInfo = imageInfos.emplace_back(VkDescriptorImageInfo
    {
        .sampler = sampler,
        .imageView = image,
        .imageLayout = layout,
    });

    const VkWriteDescriptorSet writeInfo
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = VK_NULL_HANDLE,
        .dstBinding = (uint32_t)binding,
        .descriptorCount = 1,
        .descriptorType = type,
        .pImageInfo = &imageInfo,
    };
    writes.push_back(writeInfo);
}

void blackbox::DescriptorWriter::WriteBuffer(
    int binding,
    VkBuffer buffer,
    size_t size,
    size_t offset,
    VkDescriptorType type
) {
    VkDescriptorBufferInfo& bufferInfo = bufferInfos.emplace_back(VkDescriptorBufferInfo
    {
        .buffer = buffer,
        .offset = offset,
        .range = size,
    });

    const VkWriteDescriptorSet writeInfo
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = VK_NULL_HANDLE,
        .dstBinding = (uint32_t)binding,
        .descriptorCount = 1,
        .descriptorType = type,
        .pBufferInfo = &bufferInfo,
    };
    writes.push_back(writeInfo);
}

void blackbox::DescriptorWriter::Clear()
{
    imageInfos.clear();
    writes.clear();
    bufferInfos.clear();
}

void blackbox::DescriptorWriter::UpdateSet(
    VkDevice device,
    VkDescriptorSet set
) {
    for (auto& write : writes)
    {
        write.dstSet = set;
    }
    
    vkUpdateDescriptorSets(device, (uint32_t)writes.size(), writes.data(), 0, nullptr);
}
