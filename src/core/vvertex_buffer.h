#pragma once

#include "core/vdevice.h"
#include "glm/glm.hpp"
#include <array>

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

class VVertexBuffer
{
public:

    VVertexBuffer(const VDevice& device, const std::vector<Vertex>& vertices);
    ~VVertexBuffer();

    VVertexBuffer(const VVertexBuffer&) = delete;
    VVertexBuffer& operator=(const VVertexBuffer&) = delete;

    inline const VkBuffer& buffer() const { return m_Buffer; }
    inline const std::vector<Vertex>& vertices() const { return m_Vertices; }

private:
    bool init();
    bool findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags, uint32_t& typeIndex);

    const VDevice& m_Device;

    VkBuffer m_Buffer;
    VkDeviceMemory m_Memory;

    const std::vector<Vertex>& m_Vertices;
};
