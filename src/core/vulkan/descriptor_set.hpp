#pragma once

#include "core/vulkan/device.hpp"

#include "utils/noncopyable.hpp"

namespace vrender
{

class DescriptorSetAllocator;
class Pipeline;

class DescriptorPool : private NonCopyable
{
public:
    DescriptorPool(DescriptorSetAllocator* allocator, std::vector<VkDescriptorType> descriptorTypes,
                   unsigned int descriptorCount);
    ~DescriptorPool();

    inline const std::vector<VkDescriptorSet>& descriptorSets() const { return m_DescriptorSets; }

    inline DescriptorSetAllocator* allocator() const { return m_Allocator; }

private:
    VkResult createDescriptorPool(std::vector<VkDescriptorType> types, unsigned int descriptorCount);

    VkDescriptorPool m_Pool;

    DescriptorSetAllocator* m_Allocator;

    std::vector<VkDescriptorSet> m_DescriptorSets;
};

class DescriptorSetAllocator
{
public:
    DescriptorSetAllocator(Device* device, Pipeline* pipeline);
    ~DescriptorSetAllocator();

    VkDescriptorSetLayout layout() const { return m_Layout; }

    std::vector<VkDescriptorSet> allocate(VkDescriptorPool pool, unsigned int count) const;

private:
    VkDescriptorSetLayout m_Layout;
};
}; // namespace vrender
