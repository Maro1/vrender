#pragma once

#include "core/vdevice.h"
#include "glm/glm.hpp"

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
};

class VVertexBuffer
{
public:

    VVertexBuffer(const VDevice& device, const std::vector<Vertex>& vertices);
    ~VVertexBuffer();

    VVertexBuffer(const VVertexBuffer&) = delete;
    VVertexBuffer& operator=(const VVertexBuffer&) = delete;

private:
    void init();

    const VDevice& m_Device;

    VkBuffer m_Buffer;

    const std::vector<Vertex>& m_Vertices;
};
