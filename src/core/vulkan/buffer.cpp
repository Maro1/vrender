#include "buffer.hpp"
#include "core/graphics_context.hpp"
#include "core/memory/memory_allocator.hpp"
#include "core/vulkan/command_buffer.hpp"
#include "utils/log.hpp"
#include <vulkan/vulkan_core.h>

namespace vrender
{

Buffer::Buffer(const BufferInfo& bufferInfo)
{
    m_Size = bufferInfo.size;
    m_Device = GraphicsContext::get().device();
    createBuffer(bufferInfo, m_Buffer, m_Memory);
}

Buffer::~Buffer()
{
    vkDestroyBuffer(m_Device->device(), m_Buffer, nullptr);

    GraphicsContext::get().deviceMemoryAllocator()->free(m_Memory);
}

bool Buffer::createBuffer(const BufferInfo& bufferInfo, VkBuffer& buffer, MemoryBlock& memory)
{
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
    createInfo.usage = bufferInfo.usage;
    createInfo.size = bufferInfo.size;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(m_Device->device(), &createInfo, nullptr, &buffer);
    if (result != VK_SUCCESS)
    {
        V_LOG_ERROR("Unable to create vertex buffer.");
        return false;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_Device->device(), buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    uint32_t typeIndex;
    if (!DeviceMemoryAllocator::findMemoryType(m_Device->physicalDevice(), memoryRequirements.memoryTypeBits,
                                               bufferInfo.memoryProperties, typeIndex))
    {
        V_LOG_ERROR("Failed to find required memory for buffer.");
        return false;
    }
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.memoryTypeIndex = typeIndex;
    allocInfo.allocationSize = memoryRequirements.size;

    GraphicsContext::get().deviceMemoryAllocator()->allocate(memoryRequirements.size, typeIndex, memory);

    vkBindBufferMemory(m_Device->device(), buffer, memory.memory, memory.offset);

    return true;
}

void* Buffer::copyData(void* src, size_t size, size_t offset)
{
    void* data;
    vkMapMemory(m_Device->device(), m_Memory.memory, m_Memory.offset, size, 0, &data);
    memcpy(data, src, size);
    vkUnmapMemory(m_Device->device(), m_Memory.memory);
    return data;
}

// ----------- VertexBuffer --------------
VertexBuffer::VertexBuffer(Device* device, std::vector<Vertex> vertices, std::vector<uint16_t> indices)
    : Buffer(device), m_Vertices(vertices), m_Indices(indices)
{
    createVertexBuffer();
    createIndexBuffer();
}

VertexBuffer::~VertexBuffer()
{
    vkDestroyBuffer(m_Device->device(), m_IndexBuffer, nullptr);
    GraphicsContext::get().deviceMemoryAllocator()->free(m_IndexMemory);
}

void VertexBuffer::createVertexBuffer()
{
    BufferInfo bufferInfo = {};
    bufferInfo.size = m_Vertices.size() * sizeof(Vertex);
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    createBuffer(bufferInfo, m_StagingBuffer, m_StagingMemory);

    void* data;
    vkMapMemory(m_Device->device(), m_StagingMemory.memory, m_StagingMemory.offset, bufferInfo.size, 0, &data);
    memcpy(data, m_Vertices.data(), (size_t)bufferInfo.size);
    vkUnmapMemory(m_Device->device(), m_StagingMemory.memory);

    bufferInfo.size = m_Vertices.size() * sizeof(Vertex);
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    createBuffer(bufferInfo, m_Buffer, m_Memory);
    copyBuffer(m_Buffer, bufferInfo.size);

    vkDestroyBuffer(m_Device->device(), m_StagingBuffer, nullptr);
    GraphicsContext::get().deviceMemoryAllocator()->free(m_StagingMemory);
}

void VertexBuffer::createIndexBuffer()
{
    BufferInfo bufferInfo = {};
    bufferInfo.size = m_Indices.size() * sizeof(uint16_t);
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    createBuffer(bufferInfo, m_StagingBuffer, m_StagingMemory);

    void* data;
    vkMapMemory(m_Device->device(), m_StagingMemory.memory, m_StagingMemory.offset, bufferInfo.size, 0, &data);
    memcpy(data, m_Indices.data(), (size_t)bufferInfo.size);
    vkUnmapMemory(m_Device->device(), m_StagingMemory.memory);

    bufferInfo.size = m_Indices.size() * sizeof(uint16_t);
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufferInfo.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    createBuffer(bufferInfo, m_IndexBuffer, m_IndexMemory);
    copyBuffer(m_IndexBuffer, bufferInfo.size);

    vkDestroyBuffer(m_Device->device(), m_StagingBuffer, nullptr);
    GraphicsContext::get().deviceMemoryAllocator()->free(m_StagingMemory);
}

void VertexBuffer::copyBuffer(const VkBuffer& dstBuffer, VkDeviceSize size)
{
    CommandBuffer cmdBuffer(m_Device);
    cmdBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;

    vkCmdCopyBuffer(cmdBuffer.buffer(), m_StagingBuffer, dstBuffer, 1, &copyRegion);

    cmdBuffer.submit_wait();
}

void VertexBuffer::bind(const VkCommandBuffer& commandBuffer) const
{
    VkBuffer vertexBuffers[] = {m_Buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
}

UniformBuffer::UniformBuffer(const BufferInfo& bufferInfo) : Buffer(bufferInfo)
{
    VkDescriptorBufferInfo vkBufferInfo = {};
    vkBufferInfo.buffer = m_Buffer;
    vkBufferInfo.offset = 0;
    vkBufferInfo.range = bufferInfo.size;

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = nullptr; // m_DescriptorPool.descriptorSets()[i];
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &vkBufferInfo;

    vkUpdateDescriptorSets(m_Device->device(), 1, &descriptorWrite, 0, nullptr);
}
}; // namespace vrender
