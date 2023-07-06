#include "texture.hpp"
#include "core/graphics_context.hpp"
#include "core/memory/memory_allocator.hpp"
#include "core/vulkan/image.hpp"
#include "utils/log.hpp"
#include <vulkan/vulkan_core.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace vrender
{

Texture::Texture(const std::string& filepath) : m_Device(GraphicsContext::get().device())
{
    createTextureImage(filepath);
    m_ImageView = std::make_unique<ImageView>(*m_Image, VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_SRGB);
    createSampler();
}

Texture::~Texture()
{
    vkDestroySampler(m_Device->device(), m_Sampler, nullptr);
}

bool Texture::createTextureImage(const std::string& filepath)
{
    int width, height, channels;
    stbi_uc* pixels = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!pixels)
    {
        V_LOG_ERROR("Unable to load texture at path {}", filepath);
        return false;
    }

    VkDeviceSize texSize = width * height * 4;
    BufferInfo bufferInfo = {texSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};
    Buffer buffer(bufferInfo);
    buffer.copyData(pixels, static_cast<size_t>(texSize));

    ImageInfo imageInfo;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.width = static_cast<uint32_t>(width);
    imageInfo.height = static_cast<uint32_t>(height);

    m_Image = std::make_unique<Image>(imageInfo);
    m_Image->transitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    m_Image->copyBufferToImage(buffer.buffer());
    m_Image->transitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    stbi_image_free(pixels);

    return true;
}

bool Texture::createSampler()
{
    VkSamplerCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.magFilter = VK_FILTER_LINEAR;
    createInfo.minFilter = VK_FILTER_LINEAR;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.anisotropyEnable = VK_TRUE;
    createInfo.maxAnisotropy = m_Device->limits().maxSamplerAnisotropy;
    createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    createInfo.unnormalizedCoordinates = VK_FALSE;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.mipLodBias = 0.0f;
    createInfo.minLod = 0.0f;
    createInfo.maxLod = 0.0f;

    VkResult result = vkCreateSampler(m_Device->device(), &createInfo, nullptr, &m_Sampler);
    if (result != VK_SUCCESS)
    {
        V_LOG_ERROR("Unable to create sampler.");
        return false;
    }
    return true;
}

} // namespace vrender