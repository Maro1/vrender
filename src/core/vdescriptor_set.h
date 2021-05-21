#pragma once

#include "core/vdevice.h"

class VDescriptorSetAllocator;

class VDescriptorPool
{
public:
    VDescriptorPool(VDevice* device, unsigned int descriptorCount);
    ~VDescriptorPool();

    VDescriptorPool(const VDescriptorPool&) = delete;
    VDescriptorPool& operator=(const VDescriptorPool&) = delete;

    inline const std::vector<VkDescriptorSet>& descriptorSets() const
    {
        return m_DescriptorSets;
    }

private:
    VkResult createDescriptorPool(unsigned int descriptorCount);

    VkDescriptorPool m_Pool;

    VDevice* m_Device;
    VDescriptorSetAllocator* m_Allocator;

    std::vector<VkDescriptorSet> m_DescriptorSets;
};

class VDescriptorSetAllocator
{
public:
    VDescriptorSetAllocator(VDevice* device);
    ~VDescriptorSetAllocator();

    VDescriptorSetAllocator(const VDescriptorSetAllocator&) = delete;
    VDescriptorSetAllocator& operator=(const VDescriptorSetAllocator&) = delete;

    VkDescriptorSetLayout layout() const
    {
        return m_Layout;
    }

    std::vector<VkDescriptorSet> allocate(VkDescriptorPool pool, unsigned int count) const;

private:
    VkResult createMVPLayout();

    VDevice* m_Device;

    VkDescriptorSetLayout m_Layout;
};
