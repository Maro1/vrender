#include "vvertex_buffer.h"
#include <vulkan/vulkan_core.h>

VVertexBuffer::VVertexBuffer(const VDevice& device, const std::vector<Vertex>& vertices) 
    : m_Device(device),
      m_Vertices(vertices)
{
    init();
}

VVertexBuffer::~VVertexBuffer()
{
    vkDestroyBuffer(m_Device.device(), m_Buffer, nullptr);

}

void VVertexBuffer::init()
{
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
    createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    createInfo.size = sizeof(Vertex) * m_Vertices.size();
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateBuffer(m_Device.device(), &createInfo, nullptr, &m_Buffer);

}
