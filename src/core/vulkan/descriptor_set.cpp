#include "descriptor_set.hpp"

#include "core/vulkan/pipeline.hpp"

namespace vrender
{

DescriptorPool::DescriptorPool(Device* device, DescriptorSetAllocator* allocator, unsigned int descriptorCount)
    : m_Device(device), m_Allocator(allocator)
{
    createDescriptorPool(descriptorCount + 10);
    m_DescriptorSets = m_Allocator->allocate(m_Pool, descriptorCount);
}

DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(m_Device->device(), m_Pool, nullptr);
}

VkResult DescriptorPool::createDescriptorPool(unsigned int descriptorCount)
{
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(descriptorCount);

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(descriptorCount);

    return vkCreateDescriptorPool(m_Device->device(), &poolInfo, nullptr, &m_Pool);
}

// ----------- VDescriptorSetAllocator
DescriptorSetAllocator::DescriptorSetAllocator(Device* device, Pipeline* pipeline)
    : m_Device(device), m_Layout(pipeline->descriptorSetLayout())
{
}

DescriptorSetAllocator::~DescriptorSetAllocator()
{
}

std::vector<VkDescriptorSet> DescriptorSetAllocator::allocate(VkDescriptorPool pool, unsigned int count) const
{
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkDescriptorSetLayout> layouts(count, m_Layout);
    descriptorSets.resize(count);

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = count;
    allocInfo.pSetLayouts = layouts.data();

    VkResult result = vkAllocateDescriptorSets(m_Device->device(), &allocInfo, descriptorSets.data());
    return descriptorSets;
}

}; // namespace vrender
