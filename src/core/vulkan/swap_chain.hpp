#pragma once

#include "core/vulkan/device.hpp"
#include "core/vulkan/image.hpp"
#include <memory>

namespace vrender
{
class SwapChain
{
public:
    SwapChain(Device* device, Window* window);
    ~SwapChain();

    void recreate();
    VkResult aquireNextImage(uint32_t& imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer* buffer, uint32_t imageIndex);

    inline const VkExtent2D& extent() const { return m_Extent; }
    inline const VkFormat imageFormat() const { return m_Format.format; }
    inline const VkRenderPass renderPass() const { return m_RenderPass; }
    inline const VkSwapchainKHR swapChain() const { return m_SwapChain; }
    inline const std::vector<VkFramebuffer>& framebuffers() const { return m_Framebuffers; }
    inline const std::vector<VkImage>& images() const { return m_SwapChainImages; }
    inline const VkFramebuffer framebuffer(unsigned int index) const { return m_Framebuffers[index]; }

    static constexpr unsigned int MAX_FRAMES_IN_FLIGHT = 2;

private:
    void init();
    void cleanup();

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    VkResult createImageViews();
    VkResult createRenderPass();
    VkResult createSwapChain();
    bool createSyncObjects();
    void createFramebuffers();

    Window* m_Window;
    Device* m_Device;

    VkSwapchainKHR m_SwapChain;
    VkSurfaceFormatKHR m_Format;
    VkPresentModeKHR m_PresentMode;
    VkExtent2D m_Extent;
    VkRenderPass m_RenderPass;

    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;
    std::vector<VkFramebuffer> m_Framebuffers;

    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;
    std::vector<VkFence> m_ImagesInFlight;

    unsigned int m_CurrentFrame = 0;

    Image m_DepthImage;
    ImageView m_DepthImageView;
};
}; // namespace vrender
