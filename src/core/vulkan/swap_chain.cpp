#include "swap_chain.hpp"

#include "app/window.hpp"
#include "core/vulkan/device.hpp"
#include "core/vulkan/pipeline.hpp"
#include "utils/log.hpp"

#include <algorithm>
#include <array>
#include <cstdint>

namespace vrender
{

SwapChain::SwapChain(Device* device, Window* window)
    : m_Window(window), m_Device(device),
      m_DepthImage(std::make_unique<Image>(device, ImageInfo{VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
                            window->extent().width, window->extent().height})),
      m_DepthImageView(std::make_unique<ImageView>(device, *m_DepthImage, VK_IMAGE_ASPECT_DEPTH_BIT, VK_FORMAT_D32_SFLOAT))
{
    init();
};

SwapChain::~SwapChain()
{
    cleanup();
}

void SwapChain::init()
{
    createSwapChain();
    createImageViews();
    createRenderPass();
    createFramebuffers();
    createSyncObjects();
}

void SwapChain::recreate()
{
    auto extent = m_Window->extent();
    while (extent.width == 0 || extent.height == 0)
    {
        extent = m_Window->extent();
        glfwWaitEvents();
    }
    VkResult result = vkDeviceWaitIdle(m_Device->device());

    cleanup();

    // TODO: Make depth images part of m_SwapChainImages?
    m_DepthImage = std::make_unique<Image>(m_Device, ImageInfo{VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
                            m_Window->extent().width, m_Window->extent().height});
    m_DepthImageView = std::make_unique<ImageView>(m_Device, *m_DepthImage, VK_IMAGE_ASPECT_DEPTH_BIT, VK_FORMAT_D32_SFLOAT);
    createSwapChain();
    createImageViews();
    createRenderPass();
    createFramebuffers();
    createSyncObjects();
}

void SwapChain::cleanup()
{
    for (const VkFramebuffer& framebuffer : m_Framebuffers)
    {
        vkDestroyFramebuffer(m_Device->device(), framebuffer, nullptr);
    }
    for (const VkImageView& imageView : m_SwapChainImageViews)
    {
        vkDestroyImageView(m_Device->device(), imageView, nullptr);
    }

    vkDestroySwapchainKHR(m_Device->device(), m_SwapChain, nullptr);

    vkDestroyRenderPass(m_Device->device(), m_RenderPass, nullptr);

    for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(m_Device->device(), m_ImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(m_Device->device(), m_RenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(m_Device->device(), m_InFlightFences[i], nullptr);
    }
}

VkResult SwapChain::aquireNextImage(uint32_t& imageIndex)
{
    VkResult result = vkWaitForFences(m_Device->device(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

    return vkAcquireNextImageKHR(m_Device->device(), m_SwapChain, UINT64_MAX,
                                 m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);
}

bool SwapChain::createSyncObjects()
{
    m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(m_Device->device(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) !=
            VK_SUCCESS)
            return false;
        if (vkCreateSemaphore(m_Device->device(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) !=
            VK_SUCCESS)
            return false;
        if (vkCreateFence(m_Device->device(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
            return false;
    }
    return false;
}

VkResult SwapChain::submitCommandBuffers(const VkCommandBuffer* buffer, uint32_t imageIndex)
{
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffer;

    VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(m_Device->device(), 1, &m_InFlightFences[m_CurrentFrame]);

    VkResult queueSubmitResult =
        vkQueueSubmit(m_Device->graphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]);
        
    if (queueSubmitResult != VK_SUCCESS)
        return queueSubmitResult;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrame];

    VkSwapchainKHR swapChains[] = {m_SwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    VkResult queuePresentResult = vkQueuePresentKHR(m_Device->graphicsQueue(), &presentInfo);

    if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR ||
        m_Window->framebufferResized())
    {
        m_Window->setFrameBufferResized(false);
        recreate();
    }

    m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    return queuePresentResult;
}

VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const VkSurfaceFormatKHR& format : availableFormats)
    {
        if (format.format == VK_FORMAT_B8G8R8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }
    return availableFormats.at(0);
}

VkPresentModeKHR SwapChain::choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes)
{
    for (const VkPresentModeKHR& presentMode : presentModes)
    {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return presentMode;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    VkExtent2D extent = {static_cast<uint32_t>(m_Window->width()), static_cast<uint32_t>(m_Window->height())};

    extent.width =
        std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));

    extent.height =
        std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));
    return extent;
}

VkResult SwapChain::createSwapChain()
{
    SwapChainSupportDetails supportDetails = m_Device->swapChainSupportDetails();

    m_Format = chooseSurfaceFormat(supportDetails.formats);
    m_PresentMode = choosePresentMode(supportDetails.presentModes);
    m_Extent = chooseSwapExtent(supportDetails.capabilities);

    uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;
    if (supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount)
    {
        imageCount = supportDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Device->surface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = m_Format.format;
    createInfo.presentMode = m_PresentMode;
    createInfo.imageColorSpace = m_Format.colorSpace;
    createInfo.imageExtent = m_Extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = m_Device->queueFamilyIndices();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = supportDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.clipped = true;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult swapChainResult = vkCreateSwapchainKHR(m_Device->device(), &createInfo, nullptr, &m_SwapChain);
    if (swapChainResult != VK_SUCCESS)
        return swapChainResult;

    vkGetSwapchainImagesKHR(m_Device->device(), m_SwapChain, &imageCount, nullptr);
    m_SwapChainImages.resize(imageCount);
    return vkGetSwapchainImagesKHR(m_Device->device(), m_SwapChain, &imageCount, m_SwapChainImages.data());
}

VkResult SwapChain::createImageViews()
{
    m_SwapChainImageViews.resize(m_SwapChainImages.size());

    for (unsigned int i = 0; i < m_SwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_SwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_Format.format;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.levelCount = 1;

        VkResult result = vkCreateImageView(m_Device->device(), &createInfo, nullptr, &m_SwapChainImageViews[i]);
        if (result != VK_SUCCESS)
        {
            return result;
        }
    }
    return VK_SUCCESS;
}

VkResult SwapChain::createRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = m_Format.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 0;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    return vkCreateRenderPass(m_Device->device(), &renderPassInfo, nullptr, &m_RenderPass);
}

void SwapChain::createFramebuffers()
{
    m_Framebuffers.resize(m_SwapChainImageViews.size());
    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
    {
        std::vector<VkImageView> attachments = {m_SwapChainImageViews[i], m_DepthImageView->imageView()};

        VkFramebuffer framebuffer;
        VkFramebufferCreateInfo frambufferInfo = {};
        frambufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frambufferInfo.renderPass = m_RenderPass;
        frambufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        frambufferInfo.pAttachments = attachments.data();
        frambufferInfo.width = m_Extent.width;
        frambufferInfo.height = m_Extent.height;
        frambufferInfo.layers = 1;

        vkCreateFramebuffer(m_Device->device(), &frambufferInfo, nullptr, &framebuffer);

        m_Framebuffers[i] = framebuffer;
    }
}
}; // namespace vrender
