#pragma once

#include "core/memory/memory_allocator.hpp"
#include "core/vulkan/device.hpp"

#include "glm/glm.hpp"
#include <array>

namespace vrender
{

struct Vertex
{
    glm::vec3 position;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.binding = 0;
        return bindingDescription;
    }

    // TODO: Only 1 element for now, expand when adding texcoord, normal etc
    static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescription()
    {
        std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions = {};

        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        return attributeDescriptions;
    }
};

struct BufferInfo
{
    VkDeviceSize size;
    VkBufferUsageFlags usage;
    VkMemoryPropertyFlags memoryProperties;
};

class Buffer : private NonCopyable
{
public:
    Buffer(const BufferInfo& bufferInfo);

    ~Buffer();

    void* copyData(void* src, size_t size, size_t offset = 0);

    inline const VkBuffer& buffer() const { return m_Buffer; }

protected:
    Buffer(Device* device) : m_Device(device) {}
    bool createBuffer(const BufferInfo& bufferInfo, VkBuffer& buffer, MemoryBlock& memory);

    Device* m_Device = nullptr;

    VkBuffer m_Buffer;
    MemoryBlock m_Memory;
    VkDeviceSize m_Size;

private:
    bool findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags, uint32_t& typeIndex);
};

class VertexBuffer : public Buffer
{
public:
    VertexBuffer(Device* device, std::vector<Vertex> vertices, std::vector<uint16_t> indices);
    ~VertexBuffer();

    // TODO: Change if can bind multiple at a time
    void bind(const VkCommandBuffer& commandBuffer) const;

    inline std::vector<Vertex> vertices() const { return m_Vertices; }
    inline std::vector<uint16_t> indices() const { return m_Indices; }
    inline const VkBuffer& indexBuffer() const { return m_IndexBuffer; }

private:
    void createVertexBuffer();
    void createIndexBuffer();
    void copyBuffer(const VkBuffer& dstBuffer, VkDeviceSize size);

    std::vector<Vertex> m_Vertices;
    std::vector<uint16_t> m_Indices;

    VkBuffer m_StagingBuffer;
    MemoryBlock m_StagingMemory;

    VkBuffer m_IndexBuffer;
    MemoryBlock m_IndexMemory;
};

class UniformBuffer : public Buffer
{
public:
    UniformBuffer(const BufferInfo& bufferInfo);
};

}; // namespace vrender
