#include "vvertex_buffer.h"
#include "utils/vlog.h"
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
    vkFreeMemory(m_Device.device(), m_Memory, nullptr);

}

bool VVertexBuffer::init()
{
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
    createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    createInfo.size = sizeof(Vertex) * m_Vertices.size();
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_Device.device(), &createInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
        V_LOG_ERROR("Unable to create vertex buffer.");
        return false;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_Device.device(), m_Buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    uint32_t typeIndex;
    if (!findMemoryType(memoryRequirements.memoryTypeBits, 
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, typeIndex)) 
    {
        V_LOG_ERROR("Failed to find required memory for vertex buffer.");
        return false;
    }
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.memoryTypeIndex = typeIndex;
    allocInfo.allocationSize = memoryRequirements.size;

    if (vkAllocateMemory(m_Device.device(), &allocInfo, nullptr, &m_Memory) != VK_SUCCESS) {
        V_LOG_ERROR("Unable to allocate memory for vertex buffer.");
        return false;
    }

    vkBindBufferMemory(m_Device.device(), m_Buffer, m_Memory, 0);

    void* data;
    vkMapMemory(m_Device.device(), m_Memory, 0, createInfo.size, 0, &data);
    memcpy(data, m_Vertices.data(), (size_t) createInfo.size);
    vkUnmapMemory(m_Device.device(), m_Memory);
    return true;
}

bool VVertexBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags, uint32_t& typeIndex)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(m_Device.physicalDevice(), &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & flags) == flags) {
            typeIndex = i;
            return true;
        }
    }
    return false;
}
