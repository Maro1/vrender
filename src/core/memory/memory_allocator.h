#pragma once

#include "core/device.h"
#include "utils/noncopyable.h"

#include "vulkan/vulkan.h"

#include <vector>

namespace vrender
{

struct MemoryBlock
{
    VkDeviceSize size;
    VkDeviceSize offset;

    VkDeviceMemory memory;

    bool free;

    bool operator==(const MemoryBlock& other)
    {
        return (size == other.size && offset == other.offset && memory == other.memory && free == other.free);
    }
};

class MemoryChunk : private NonCopyable
{
public:
    bool allocateChunk(const Device& device, VkDeviceSize size, uint32_t memoryTypeIndex);

    bool allocateBlock(VkDeviceSize size, MemoryBlock& block);
    bool freeBlock(const MemoryBlock& block);

    inline uint32_t memoryTypeIndex() const { return m_MemoryTypeIndex; }

private:
    VkDeviceMemory m_Memory;
    VkDeviceSize m_Size;
    uint32_t m_MemoryTypeIndex;

    void* m_Ptr = nullptr;

    std::vector<MemoryBlock> m_Blocks;
};

class MemoryAllocator : private NonCopyable
{
public:
    MemoryAllocator(Device* device, VkDeviceSize size);

    bool allocate(VkDeviceSize size, uint32_t memoryTypeIndex, MemoryBlock& block);
    void free(const MemoryBlock& block);

private:
    static inline bool isPowerOfTwo(uint64_t num) { return (num & (num - 1)) == 0 && num != 0; };
    static uint64_t nextPowerOfTwo(uint64_t num);

    std::vector<MemoryChunk> m_Chunks;
    Device* m_Device;
    VkDeviceSize m_Size;

    uint64_t m_PreferredChunkSize = 512;
};
}; // namespace vrender
