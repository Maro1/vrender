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
    DescriptorPool(Device* device, DescriptorSetAllocator* allocator, unsigned int descriptorCount);
    ~DescriptorPool();

    inline const std::vector<VkDescriptorSet>& descriptorSets() const { return m_DescriptorSets; }

    inline DescriptorSetAllocator* allocator() const { return m_Allocator; }

private:
    VkResult createDescriptorPool(unsigned int descriptorCount);

    VkDescriptorPool m_Pool;

    Device* m_Device;
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
    Device* m_Device;

    VkDescriptorSetLayout m_Layout;
};
}; // namespace vrender
