#include "memory_allocator.hpp"
#include "utils/log.hpp"
#include <vulkan/vulkan_core.h>

namespace vrender
{

MemoryAllocation::MemoryAllocation(Device* device, VkDeviceSize size, uint32_t memoryTypeIndex)
    : m_Device(device->device()), m_Size(size)
{
    MemoryBlock initialBlock;
    initialBlock.typeIndex = memoryTypeIndex;
    initialBlock.size = size;
    initialBlock.free = true;
    initialBlock.offset = 0;

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
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

    m_Blocks.push_back(initialBlock);
}

MemoryAllocation::~MemoryAllocation()
{
    vkFreeMemory(m_Device, m_Memory, nullptr);
}

bool MemoryAllocation::freeBlock(const MemoryBlock& block)
{
    // TODO: MERGE CONTINUOUS BLOCKS
    auto b = std::find(m_Blocks.begin(), m_Blocks.end(), block);
    if (b == m_Blocks.end())
        return false;
    b->free = true;
    return true;
}

bool MemoryAllocation::allocateBlock(VkDeviceSize size, VkDeviceSize alignment, MemoryBlock& rblock)
{
    if (size > m_Size)
        return false;

    for (unsigned int i = 0; i < m_Blocks.size(); i++)
    {
        if (m_Blocks[i].free)
        {
            if (m_Blocks[i].offset % alignment != 0)
            {
                VkDeviceSize padding = (alignment - (m_Blocks[i].offset % alignment));
                VkDeviceSize requiredSize = size + padding;
                if (m_Blocks[i].size < requiredSize)
                {
                    continue;
                }
                VkDeviceSize newOffset = m_Blocks[i].offset + padding;
                m_Blocks[i].offset = newOffset;
                m_Blocks[i].size -= padding;
            }
            if (m_Blocks[i].size == size)
            {
                // Found block of same size, allocate it
                m_Blocks[i].free = false;
                rblock = m_Blocks[i];
                m_AllocatedSize += size;
                return true;
            }
            if (m_Blocks[i].size > size)
            {
                // Found block of bigger size, split it and allocate
                MemoryBlock newBlock;
                newBlock.size = m_Blocks[i].size - size;
                newBlock.offset = m_Blocks[i].offset + size;
                newBlock.memory = m_Memory;
                newBlock.typeIndex = m_Blocks[i].typeIndex;
                newBlock.free = true;
                m_Blocks.push_back(newBlock);

                m_Blocks[i].size = size;
                m_Blocks[i].free = false;
                rblock = m_Blocks[i];
                m_AllocatedSize += size;
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

DeviceMemoryAllocator::~DeviceMemoryAllocator()
{
    for (const MemoryAllocation* alloc : m_Allocations)
    {
        if (!alloc)
            continue;

        const MemoryAllocation* next = alloc;
        do
        {
            next = alloc->next();
            delete alloc;
            alloc = next;
        } while (next);
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
        while (allocation->next() != nullptr)
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

    while (allocation && allocation->remainingSize() < size)
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

bool DeviceMemoryAllocator::allocate(VkDeviceSize size, VkDeviceSize alignment, uint32_t memoryTypeIndex,
                                     MemoryBlock& block)
{
    VkDeviceSize requestSize = ((size / m_PageSize) + 1) * m_PageSize;

    MemoryAllocation* suitableAllocation = findSuitableAllocation(requestSize, memoryTypeIndex);

    return suitableAllocation->allocateBlock(requestSize, alignment, block);
}

void DeviceMemoryAllocator::free(const MemoryBlock& block)
{
    m_Allocations[block.typeIndex]->freeBlock(block);
}

bool DeviceMemoryAllocator::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                                           VkMemoryPropertyFlags flags, uint32_t& typeIndex)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
        {
            typeIndex = i;
            return true;
        }
    }
    return false;
}

}; // namespace vrender
