#pragma once

#include "core/rendering/renderer.hpp"
#include "core/vulkan/buffer.hpp"
#include "core/vulkan/texture.hpp"
#include "core/vulkan/uniform.hpp"
#include "ecs/system.hpp"
#include "glm/fwd.hpp"
#include "scene/scene.hpp"

namespace vrender
{

const std::vector<VkDescriptorType> DESCRIPTOR_TYPES = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};

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
    MeshRenderSystem(Device* device, SwapChain* swapChain, Window* window, Scene* scene);

    virtual void start() override;
    virtual void update() override;

private:
    Renderer m_Renderer;
    UniformHandler m_GlobalUniformHandler;

    DescriptorSetAllocator m_DescriptorAllocator;
    DescriptorPool m_DescriptorPool;

    Texture m_Texture; // TODO: Make into component or something instead
};
} // namespace vrender