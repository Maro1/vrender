#pragma once

#include <memory>
#include <string>
#include <vulkan/vulkan_core.h>

#include "buffer.hpp"
#include "core/memory/memory_allocator.hpp"
#include "image.hpp"

namespace vrender
{

class Texture
{
public:
    Texture(const std::string& filepath);
    ~Texture();

    inline VkImageView imageView() const { return m_ImageView->imageView(); }
    inline VkSampler sampler() const { return m_Sampler; }

private:
    bool createTextureImage(const std::string& filepath);
    bool createImageView();
    bool createSampler();

    MemoryBlock m_Memory;

    std::unique_ptr<Image> m_Image;
    std::unique_ptr<ImageView> m_ImageView;

    VkSampler m_Sampler;

    Device* m_Device;
};
} // namespace vrender