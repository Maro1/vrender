#include "image.hpp"
#include "core/graphics_context.hpp"
#include "core/vulkan/command_buffer.hpp"
#include "utils/log.hpp"
#include <vulkan/vulkan_core.h>

namespace vrender
{

Image::Image(Device* device, const ImageInfo& imageInfo) : m_Device(device), m_Info(imageInfo)
{
    createImage(imageInfo, m_Image, m_Memory);
}

Image::~Image()
{
    vkDestroyImage(m_Device->device(), m_Image, nullptr);

    GraphicsContext::get().deviceMemoryAllocator()->free(m_Memory);
}

void Image::transitionLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    CommandBuffer cmdBuffer(m_Device);
    cmdBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_Image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        V_LOG_ERROR("Unsupported image layout transition");
    }

    vkCmdPipelineBarrier(cmdBuffer.buffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    cmdBuffer.submit_wait();
}

void Image::copyBufferToImage(VkBuffer buffer)
{
    CommandBuffer cmdBuffer(m_Device);
    cmdBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferImageCopy region = {};

    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {m_Info.width, m_Info.height, 1};

    vkCmdCopyBufferToImage(cmdBuffer.buffer(), buffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    cmdBuffer.submit_wait();
}

bool Image::createImage(const ImageInfo& imageInfo, VkImage& image, MemoryBlock& memory)
{
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = imageInfo.width;
    imageCreateInfo.extent.height = imageInfo.height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = imageInfo.format;
    imageCreateInfo.tiling = imageInfo.tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = imageInfo.usage;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    vkCreateImage(m_Device->device(), &imageCreateInfo, nullptr, &m_Image);

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(m_Device->device(), m_Image, &memoryRequirements);

    uint32_t typeIndex;
    if (!DeviceMemoryAllocator::findMemoryType(m_Device->physicalDevice(), memoryRequirements.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, typeIndex))
    {
        V_LOG_ERROR("Failed to find required memory for texture.");
        return false;
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = typeIndex;

    GraphicsContext::get().deviceMemoryAllocator()->allocate(memoryRequirements.size, typeIndex, m_Memory);

    vkBindImageMemory(m_Device->device(), m_Image, m_Memory.memory, 0);

    return true;
}

// ImageView
ImageView::ImageView(Device* device, const Image& image, VkImageAspectFlags aspectFlags) : m_Device(device)
{
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image.image();
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    vkCreateImageView(device->device(), &createInfo, nullptr, &m_ImageView);
}

ImageView::~ImageView()
{
    vkDestroyImageView(m_Device->device(), m_ImageView, nullptr);
}

} // namespace vrender