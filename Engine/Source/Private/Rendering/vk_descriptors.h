#pragma once

#include "vk_types.h"

struct DescriptorLayoutBuilder
{
    std::vector<VkDescriptorSetLayoutBinding> Bindings {};

    void AddBinding(uint32_t Binding, VkDescriptorType Type);
    void Clear();
    VkDescriptorSetLayout Build(VkDevice Device, VkShaderStageFlags ShaderStages, const void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags Flags = 0);
};

struct DescriptorAllocator
{
    struct PoolSizeRatio
    {
        VkDescriptorType Type {};
        float Ratio {};
    };

    VkDescriptorPool Pool {};

    void InitPool(VkDevice Device, uint32_t MaxSets, std::span<PoolSizeRatio> PoolRatios);
    void ClearDescriptors(VkDevice Device);
    void DestroyPool(VkDevice Device);

    VkDescriptorSet Allocate(VkDevice Device, VkDescriptorSetLayout Layout);
};
