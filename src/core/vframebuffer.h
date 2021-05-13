#pragma once

#include "core/vdevice.h"

class VFramebuffer
{
public:

    VFramebuffer(
            const VDevice& device, 
            const VkRenderPass& renderPass,
            uint32_t width,
            uint32_t height,
            const std::vector<VkImageView>& attachments);
    
    ~VFramebuffer();

    VFramebuffer(const VFramebuffer&) = delete;
    VFramebuffer& operator=(const VFramebuffer&) = delete;

    inline const VkFramebuffer& framebuffer() const { return m_Framebuffer; }

private:

    const VDevice& m_Device;

    VkFramebuffer m_Framebuffer;


};
