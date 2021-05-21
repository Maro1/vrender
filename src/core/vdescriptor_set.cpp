#include "vdescriptor_set.h"

VDescriptorPool::VDescriptorPool(VDevice* device, unsigned int descriptorCount) : m_Device(device)
{
    m_Allocator = new VDescriptorSetAllocator(device);
    createDescriptorPool(descriptorCount);
    m_DescriptorSets = m_Allocator->allocate(m_Pool, descriptorCount);
}

VDescriptorPool::~VDescriptorPool()
{
    delete m_Allocator;
    vkDestroyDescriptorPool(m_Device->device(), m_Pool, nullptr);
}

VkResult VDescriptorPool::createDescriptorPool(unsigned int descriptorCount)
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
VDescriptorSetAllocator::VDescriptorSetAllocator(VDevice* device) : m_Device(device)
{
    createMVPLayout();
}

VDescriptorSetAllocator::~VDescriptorSetAllocator()
{
    vkDestroyDescriptorSetLayout(m_Device->device(), m_Layout, nullptr);
}

std::vector<VkDescriptorSet> VDescriptorSetAllocator::allocate(VkDescriptorPool pool, unsigned int count) const
{
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkDescriptorSetLayout> layouts(count, m_Layout);
    descriptorSets.resize(count);

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = count;
    allocInfo.pSetLayouts = layouts.data();

    vkAllocateDescriptorSets(m_Device->device(), &allocInfo, descriptorSets.data());
    return descriptorSets;
}

VkResult VDescriptorSetAllocator::createMVPLayout()
{
    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = 0;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &layoutBinding;

    return vkCreateDescriptorSetLayout(m_Device->device(), &layoutInfo, nullptr, &m_Layout);
}

