#pragma once

#include "core/rendering/renderer.hpp"
#include "core/vulkan/buffer.hpp"
#include "core/vulkan/uniform.hpp"
#include "ecs/system.hpp"
#include "glm/fwd.hpp"
#include "scene/scene.hpp"

namespace vrender
{

constexpr uint32_t FRAME_OVERLAP = 2;

struct GlobalUBO
{
    glm::mat4 viewProjection;
};

struct PushData
{
    glm::mat4 model;
};

class RenderSystem : private NonCopyable
{
public:
    RenderSystem();
    virtual ~RenderSystem() = default;
};

class MeshRenderSystem : public System
{
public:
    MeshRenderSystem(Device* device, SwapChain* swapChain, Window* window, Scene* scene)
        : System(scene), m_Renderer(device, swapChain, window), m_DescriptorAllocator(device, &m_Renderer.pipeline()),
          m_DescriptorPool(device, &m_DescriptorAllocator, FRAME_OVERLAP), m_GlobalUniformHandler(sizeof(GlobalUBO))
    {
        for (uint32_t i = 0; i < FRAME_OVERLAP; i++)
        {
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = m_GlobalUniformHandler.buffer()->buffer();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(GlobalUBO);

            VkWriteDescriptorSet descriptorWrite = {};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = m_DescriptorPool.descriptorSets()[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(device->device(), 1, &descriptorWrite, 0, nullptr);
        }
    }

    virtual void start() override;
    virtual void update() override;

private:
    Renderer m_Renderer;
    UniformHandler m_GlobalUniformHandler;

    DescriptorSetAllocator m_DescriptorAllocator;
    DescriptorPool m_DescriptorPool;
};
} // namespace vrender