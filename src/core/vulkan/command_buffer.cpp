#include "command_buffer.hpp"
#include <vulkan/vulkan_core.h>

namespace vrender
{
CommandBuffer::CommandBuffer(Device* device) : m_Device(device)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = m_Device->commandPool();

    vkAllocateCommandBuffers(m_Device->device(), &allocInfo, &m_CommandBuffer);
}

CommandBuffer::~CommandBuffer()
{
    vkFreeCommandBuffers(m_Device->device(), m_Device->commandPool(), 1, &m_CommandBuffer);
}

void CommandBuffer::begin(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags;

    vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
}

void CommandBuffer::submit_wait()
{
    vkEndCommandBuffer(m_CommandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffer;

    vkQueueSubmit(m_Device->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_Device->graphicsQueue());
}

void CommandBuffer::submit()
{
    vkEndCommandBuffer(m_CommandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffer;

    vkQueueSubmit(m_Device->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
}
} // namespace vrender