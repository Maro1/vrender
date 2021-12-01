#pragma once

#include "core/vulkan/buffer.hpp"
#include "core/vulkan/device.hpp"
#include "core/vulkan/pipeline.hpp"
#include "core/vulkan/swap_chain.hpp"

#include "scene/camera/arcball_camera.hpp"
#include "scene/camera/camera.hpp"
#include "scene/scene.hpp"

#include "utils/noncopyable.hpp"

#include <vector>

namespace vrender
{

struct FrameData
{
    VkSemaphore presentSemaphore, renderSemaphore;
    VkFence inFlightFence;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
};

class Renderer : private NonCopyable
{
public:
    Renderer(Device* device, SwapChain* swapChain, Window* window);
    ~Renderer();

    VkCommandBuffer beginFrame();
    VkResult endFrame();
    void beginRenderPass();
    void endRenderPass();

    inline Pipeline& pipeline() { return m_Pipeline; }
    inline SwapChain* swapChain() { return m_SwapChain; }
    inline uint32_t currentFrame() const { return m_CurrentFrame; }

protected:
    void init();

    VkResult createCommandBuffers();

    Device* m_Device;
    SwapChain* m_SwapChain;
    Window* m_Window;

    Pipeline m_Pipeline;

    std::vector<VkCommandBuffer> m_CommandBuffers;
    uint32_t m_CurrentFrame = 0;
    uint32_t m_CurrentImage;
};

}; // namespace vrender
