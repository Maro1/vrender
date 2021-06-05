#include "memory_allocator.h"
#include "utils/log.h"

namespace vrender
{

MemoryAllocation::MemoryAllocation(Device* device, VkDeviceSize size, uint32_t memoryTypeIndex)
{
    MemoryBlock initialBlock;
    initialBlock.size = size;
    initialBlock.free = true;
    initialBlock.offset = 0;

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.allocationSize = size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    VkResult result = vkAllocateMemory(device->device(), &allocInfo, nullptr, &m_Memory);
    if (result != VK_SUCCESS)
    {
        V_LOG_ERROR("Unable to allocate memory, error code: {}", result);
        return;
    }

    initialBlock.memory = m_Memory;

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(device->physicalDevice(), &memoryProperties);

    if ((memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        vkMapMemory(device->device(), m_Memory, initialBlock.offset, VK_WHOLE_SIZE, 0, &m_Ptr);

    m_Blocks.push_back(initialBlock);
}

bool MemoryAllocation::freeBlock(const MemoryBlock& block)
{
    auto b = std::find(m_Blocks.begin(), m_Blocks.end(), block);
    if (b == m_Blocks.end())
        return false;
    b->free = true;
    return true;
}

bool MemoryAllocation::allocateBlock(VkDeviceSize size, MemoryBlock& rblock)
{
    if (size > m_Size)
        return false;

    for (auto& block : m_Blocks)
    {
        if (block.free)
        {
            if (block.size == size)
            {
                // Found block of same size, allocate it
                block.free = false;
                rblock = block;
                return true;
            }
            if (block.size > size)
            {
                // Found block of bigger size, split it and allocate
                MemoryBlock newBlock;
                newBlock.size = block.size - size;
                newBlock.offset = block.offset + size;
                newBlock.memory = m_Memory;
                newBlock.free = true;
                m_Blocks.push_back(newBlock);

                block.size = size;
                block.free = false;
                rblock = block;
                return true;
            }
        }
    }
    return false;
}

DeviceMemoryAllocator::DeviceMemoryAllocator(Device* device, VkDeviceSize size) : m_Device(device)
{
    m_MemoryProperties = m_Device->memoryProperties();
    m_PageSize = m_Device->limits().bufferImageGranularity;
    m_MinimumAllocationSize = m_PageSize * 10;

    m_Allocations.resize(m_MemoryProperties.memoryTypeCount);

    m_MemoryTypes.resize(m_MemoryProperties.memoryTypeCount);
    for (unsigned int i = 0; i < m_MemoryProperties.memoryTypeCount; i++)
    {
        m_MemoryTypes[i] = m_MemoryProperties.memoryTypes[i];
    }

    m_MemoryHeaps.resize(m_MemoryProperties.memoryHeapCount);
    for (unsigned int i = 0; i < m_MemoryProperties.memoryHeapCount; i++)
    {
        m_MemoryHeaps[i] = {};
        m_MemoryHeaps[i].size = m_MemoryProperties.memoryHeaps[i].size;
        m_MemoryHeaps[i].flags = m_MemoryProperties.memoryHeaps[i].flags;
        m_MemoryHeaps[i].budgetSize = m_MemoryHeaps[i].size - (m_MemoryHeaps[i].size >> 2); // 75%
    }
}

uint64_t DeviceMemoryAllocator::nextPowerOfTwo(uint64_t num)
{
    num--;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
    num |= num >> 32;
    num++;
    return num;
}

MemoryAllocation* DeviceMemoryAllocator::allocateNewMemory(VkDeviceSize size, uint32_t memoryTypeIndex)
{
    MemoryAllocation* allocation = m_Allocations[memoryTypeIndex];
    VkDeviceSize allocSize;

    if (!allocation)
    {
        allocSize = size > m_MinimumAllocationSize ? size : m_MinimumAllocationSize;
        MemoryAllocation* alloc = new MemoryAllocation(m_Device, allocSize, memoryTypeIndex);
        m_Allocations[memoryTypeIndex] = alloc;
        return alloc;
    }
    else
    {
        while (allocation != nullptr)
            allocation = allocation->next();

        allocSize = size > (allocation->size() * 2) ? size : allocation->size() * 2;

        MemoryAllocation* alloc = new MemoryAllocation(m_Device, allocSize, memoryTypeIndex);
        allocation->setNext(alloc);
        return alloc;
    }
}

MemoryAllocation* DeviceMemoryAllocator::findSuitableAllocation(VkDeviceSize size, uint32_t memoryTypeIndex)
{
    MemoryAllocation* allocation = m_Allocations[memoryTypeIndex];

    if (!allocation)
    {
        // Initial allocation for this memory type
        allocation = allocateNewMemory(size * 2, memoryTypeIndex);
        return allocation;
    }

    while (allocation->remainingSize() < size)
    {
        allocation = allocation->next();
    }

    if (!allocation)
    {
        // No allocation has space, allocate new
        allocation = allocateNewMemory(size * 2, memoryTypeIndex);
    }

    return allocation;
}

bool DeviceMemoryAllocator::allocate(VkDeviceSize size, uint32_t memoryTypeIndex, MemoryBlock& block)
{
    VkDeviceSize requestSize = ((size / m_PageSize) + 1) * m_PageSize;
    MemoryAllocation* suitableAllocation = findSuitableAllocation(requestSize, memoryTypeIndex);

    return suitableAllocation->allocateBlock(requestSize, block);
}

}; // namespace vrender
