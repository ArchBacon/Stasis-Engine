#include "vk_descriptors.h"

void DescriptorLayoutBuilder::AddBinding(
    const uint32_t Binding,
    const VkDescriptorType Type
) {
    VkDescriptorSetLayoutBinding NewBinding {};
    NewBinding.binding = Binding;
    NewBinding.descriptorCount = 1;
    NewBinding.descriptorType = Type;

    Bindings.push_back(NewBinding);   
}

void DescriptorLayoutBuilder::Clear()
{
    Bindings.clear();
}

VkDescriptorSetLayout DescriptorLayoutBuilder::Build(
    const VkDevice Device,
    const VkShaderStageFlags ShaderStages,
    const void* pNext,
    const VkDescriptorSetLayoutCreateFlags Flags
) {
    for (auto& Binding : Bindings)
    {
        Binding.stageFlags |= ShaderStages;
    }

    VkDescriptorSetLayoutCreateInfo LayoutInfo { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    LayoutInfo.pNext = pNext;

    LayoutInfo.pBindings = Bindings.data();
    LayoutInfo.bindingCount = static_cast<uint32_t>(Bindings.size());
    LayoutInfo.flags = Flags;

    VkDescriptorSetLayout Set {};
    VK_CHECK(vkCreateDescriptorSetLayout(Device, &LayoutInfo, nullptr, &Set));

    return Set;
}

void DescriptorAllocator::InitPool(
    const VkDevice Device,
    const uint32_t MaxSets,
    const std::span<PoolSizeRatio> PoolRatios
) {
    std::vector<VkDescriptorPoolSize> PoolSizes {};
    for (const auto [Type, Ratio] : PoolRatios)
    {
        PoolSizes.push_back(VkDescriptorPoolSize {
            .type = Type,
            .descriptorCount = static_cast<uint32_t>(Ratio * MaxSets)
        });
    }

    VkDescriptorPoolCreateInfo PoolInfo { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    PoolInfo.flags = 0;
    PoolInfo.maxSets = MaxSets;
    PoolInfo.poolSizeCount = static_cast<uint32_t>(PoolSizes.size());
    PoolInfo.pPoolSizes = PoolSizes.data();

    vkCreateDescriptorPool(Device, &PoolInfo, nullptr, &Pool);
}

void DescriptorAllocator::ClearDescriptors(
    const VkDevice Device
) {
    vkResetDescriptorPool(Device, Pool, 0);
}

void DescriptorAllocator::DestroyPool(
    const VkDevice Device
) {
    vkDestroyDescriptorPool(Device, Pool, nullptr);
}

VkDescriptorSet DescriptorAllocator::Allocate(
    const VkDevice Device,
    const VkDescriptorSetLayout Layout
) {
    VkDescriptorSetAllocateInfo AllocInfo { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    AllocInfo.pNext = nullptr;
    AllocInfo.descriptorPool = Pool;
    AllocInfo.descriptorSetCount = 1;
    AllocInfo.pSetLayouts = &Layout;

    VkDescriptorSet DescriptorSet {};
    VK_CHECK(vkAllocateDescriptorSets(Device, &AllocInfo, &DescriptorSet));

    return DescriptorSet;
}
