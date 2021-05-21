#include "vframebuffer.h"
VFramebuffer::VFramebuffer(
        VDevice* device, 
        const VkRenderPass& renderPass,
        uint32_t width,
        uint32_t height,
        const std::vector<VkImageView>& attachments)
    : m_Device(device)
{
    VkFramebufferCreateInfo frambufferInfo = {};
    frambufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frambufferInfo.renderPass = renderPass;
    frambufferInfo.attachmentCount = 1;
    frambufferInfo.pAttachments = attachments.data();
    frambufferInfo.width = width;
    frambufferInfo.height = height;
    frambufferInfo.layers = 1;

    vkCreateFramebuffer(m_Device->device(), &frambufferInfo, nullptr, &m_Framebuffer);
}

VFramebuffer::~VFramebuffer()
{
    vkDestroyFramebuffer(m_Device->device(), m_Framebuffer, nullptr);
}

