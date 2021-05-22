#pragma once

#include "core/device.h"

namespace vrender
{
class Framebuffer
{
public:
    Framebuffer(Device* device, const VkRenderPass& renderPass, uint32_t width, uint32_t height,
                const std::vector<VkImageView>& attachments);

    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    inline const VkFramebuffer& framebuffer() const { return m_Framebuffer; }

private:
    Device* m_Device;

    VkFramebuffer m_Framebuffer;
};
}; // namespace vrender
