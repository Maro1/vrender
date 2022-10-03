#pragma once

#include "core/memory/memory_allocator.hpp"
#include "core/vulkan/device.hpp"
#include <vulkan/vulkan_core.h>

namespace vrender
{

struct ImageInfo
{
    VkImageUsageFlags usage;
    VkFormat format;
    VkImageTiling tiling;
    uint32_t width;
    uint32_t height;
};

class Image : private NonCopyable
{
public:
    Image(Device* device, const ImageInfo& bufferInfo);

    ~Image();

    void transitionLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer);

    inline const VkImage& image() const { return m_Image; }

protected:
    bool createImage(const ImageInfo& bufferInfo, VkImage& image, MemoryBlock& memory);

    Device* m_Device;

    VkImage m_Image;
    MemoryBlock m_Memory;
    ImageInfo m_Info;
};

class ImageView : private NonCopyable
{
public:
    ImageView(Device* device, const Image& image, VkImageAspectFlags aspectFlags);

    ~ImageView();

    inline const VkImageView& imageView() const { return m_ImageView; }

private:
    Device* m_Device;
    VkImageView m_ImageView;
};

}; // namespace vrender
