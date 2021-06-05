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

class MemoryAllocation : private NonCopyable
{
public:
    bool allocate(const Device& device, VkDeviceSize size, uint32_t memoryTypeIndex);

    MemoryAllocation(Device* device, VkDeviceSize size, uint32_t memoryTypeIndex);

    bool allocateBlock(VkDeviceSize size, MemoryBlock& block);
    bool freeBlock(const MemoryBlock& block);

    inline uint32_t memoryTypeIndex() const { return m_MemoryTypeIndex; }
    inline VkDeviceSize remainingSize() const { return m_Size - m_AllocatedSize; }
    inline MemoryAllocation* next() const { return m_Next; }
    inline VkDeviceSize size() const { return m_Size; }
    void setNext(MemoryAllocation* next) { m_Next = next; }

private:
    VkDeviceMemory m_Memory;
    VkDeviceSize m_Size;
    VkDeviceSize m_AllocatedSize;
    uint32_t m_MemoryTypeIndex;

    void* m_Ptr = nullptr;

    std::vector<MemoryBlock> m_Blocks;
    MemoryAllocation* m_Next;
};

struct Heap
{
    VkDeviceSize size;
    VkDeviceSize budgetSize;
    VkMemoryHeapFlags flags;
};

class DeviceMemoryAllocator : private NonCopyable
{
public:
    DeviceMemoryAllocator(Device* device, VkDeviceSize size);

    bool allocate(VkDeviceSize size, uint32_t memoryTypeIndex, MemoryBlock& block);
    void free(const MemoryBlock& block);

private:
    MemoryAllocation* allocateNewMemory(VkDeviceSize size, uint32_t memoryTypeIndex);
    MemoryAllocation* findSuitableAllocation(VkDeviceSize size, uint32_t memoryTypeIndex);

    static inline bool isPowerOfTwo(uint64_t num) { return (num & (num - 1)) == 0 && num != 0; };
    static uint64_t nextPowerOfTwo(uint64_t num);

    std::vector<MemoryAllocation*> m_Allocations;
    Device* m_Device;
    VkDeviceSize m_Size;
    VkPhysicalDeviceMemoryProperties m_MemoryProperties;

    std::vector<VkMemoryType> m_MemoryTypes;
    std::vector<Heap> m_MemoryHeaps;

    VkDeviceSize m_PageSize;
    VkDeviceSize m_MinimumAllocationSize;
};
}; // namespace vrender
