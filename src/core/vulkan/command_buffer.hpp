#pragma once

#include "core/vulkan/device.hpp"

namespace vrender
{

class CommandBuffer
{
public:
    CommandBuffer(Device* device);
    ~CommandBuffer();

    inline const VkCommandBuffer buffer() { return m_CommandBuffer; }

    void begin(VkCommandBufferUsageFlags flags);
    void submit_wait();
    void submit();

private:
    VkCommandBuffer m_CommandBuffer;
    Device* m_Device;
};
} // namespace vrender