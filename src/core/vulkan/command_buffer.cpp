#include "command_buffer.hpp"

#include "core/graphics_context.hpp"

namespace vrender
{
CommandBuffer::CommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = GraphicsContext::get().device()->commandPool();

    vkAllocateCommandBuffers(GraphicsContext::get().device()->device(), &allocInfo, &m_CommandBuffer);
}

CommandBuffer::~CommandBuffer()
{
    vkFreeCommandBuffers(GraphicsContext::get().device()->device(), GraphicsContext::get().device()->commandPool(), 1, &m_CommandBuffer);
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

    vkQueueSubmit(GraphicsContext::get().device()->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(GraphicsContext::get().device()->graphicsQueue());
}

void CommandBuffer::submit()
{
    vkEndCommandBuffer(m_CommandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffer;

    vkQueueSubmit(GraphicsContext::get().device()->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
}
} // namespace vrender