#include "memory_allocator.h"
#include "utils/log.h"

namespace vrender
{

bool MemoryChunk::allocateChunk(const Device& device, VkDeviceSize size, uint32_t memoryTypeIndex)
{
    if ((size & (size - 1)) != 0 || size == 0)
    {
        V_LOG_ERROR("Chunk size must be a power of two.");
        return false;
    }
    MemoryBlock initialBlock;
    initialBlock.size = size;
    initialBlock.free = true;
    initialBlock.offset = 0;

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.allocationSize = size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    VkResult result = vkAllocateMemory(device.device(), &allocInfo, nullptr, &m_Memory);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    initialBlock.memory = m_Memory;

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(device.physicalDevice(), &memoryProperties);

    if ((memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        vkMapMemory(device.device(), m_Memory, initialBlock.offset, VK_WHOLE_SIZE, 0, &m_Ptr);

    m_Blocks.push_back(initialBlock);
    return true;
}

bool MemoryChunk::freeBlock(const MemoryBlock& block)
{
    auto b = std::find(m_Blocks.begin(), m_Blocks.end(), block);
    if (b == m_Blocks.end())
        return false;
    b->free = true;
    return true;
}

bool MemoryChunk::allocateBlock(VkDeviceSize size, MemoryBlock& rblock)
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

MemoryAllocator::MemoryAllocator(Device* device, VkDeviceSize size) : m_Device(device) {}

uint64_t MemoryAllocator::nextPowerOfTwo(uint64_t num)
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

bool MemoryAllocator::allocate(VkDeviceSize size, uint32_t memoryTypeIndex, MemoryBlock& block)
{
    for (MemoryChunk& chunk : m_Chunks)
    {
        if (chunk.memoryTypeIndex() == memoryTypeIndex)
        {
            if (chunk.allocateBlock(size, block))
                return true;
        }
    }

    if (!isPowerOfTwo(size))
    {
        size = nextPowerOfTwo(size);
    }

    uint64_t chunkSize = m_PreferredChunkSize;
    while (chunkSize > size)
    {
        MemoryChunk chunk;
        if (chunk.allocateChunk(*m_Device, chunkSize, memoryTypeIndex))
        {
            if (chunk.allocateBlock(size, block))
                return true;
        }
        chunkSize = chunkSize >> 2;
    };
    return false;
}

}; // namespace vrender
