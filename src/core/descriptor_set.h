#pragma once

#include "core/device.h"

namespace vrender
{

class DescriptorSetAllocator;

class DescriptorPool
{
public:
    DescriptorPool(Device* device, unsigned int descriptorCount);
    ~DescriptorPool();

    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

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
    DescriptorSetAllocator(Device* device);
    ~DescriptorSetAllocator();

    DescriptorSetAllocator(const DescriptorSetAllocator&) = delete;
    DescriptorSetAllocator& operator=(const DescriptorSetAllocator&) = delete;

    VkDescriptorSetLayout layout() const { return m_Layout; }

    std::vector<VkDescriptorSet> allocate(VkDescriptorPool pool, unsigned int count) const;

private:
    VkResult createMVPLayout();

    Device* m_Device;

    VkDescriptorSetLayout m_Layout;
};
}; // namespace vrender
