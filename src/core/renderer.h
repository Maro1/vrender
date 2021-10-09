#pragma once

#include "core/buffer.h"
#include "core/device.h"
#include "core/pipeline.h"
#include "core/swap_chain.h"

#include "scene/camera/arcball_camera.h"
#include "scene/camera/camera.h"
#include "scene/scene.h"

#include "utils/noncopyable.h"

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
    void draw(const VkCommandBuffer& commandBuffer); // TODO: MOVE TO SCENE GRAPH

    inline Pipeline& pipeline() { return m_Pipeline; }
    inline CameraController& cameraController() { return m_CameraController; }

protected:
    void init();

    VkResult createCommandBuffers();
    void updateMVP();

    Device* m_Device;
    SwapChain* m_SwapChain;
    Window* m_Window;

    Pipeline m_Pipeline;
    std::unique_ptr<Camera> m_Camera;
    ArcballCameraController m_CameraController;

    std::vector<VkCommandBuffer> m_CommandBuffers;
    uint32_t m_CurrentFrame = 0;
    uint32_t m_CurrentImage;

    std::vector<std::unique_ptr<VertexBuffer>> m_VertexBuffers;
    std::vector<std::unique_ptr<Buffer>> m_MVPBuffers;

    const std::vector<Vertex> vertices = {
        {.position = glm::vec3(-0.5f, -0.5f, 0.0f)}, {.position = glm::vec3(0.5f, -0.5f, 0.0f)},
        {.position = glm::vec3(0.5f, 0.5f, 0.0f)},   {.position = glm::vec3(-0.5f, 0.5f, 0.0f)},

        {.position = glm::vec3(-0.5f, -0.5f, 1.0f)}, {.position = glm::vec3(0.5f, -0.5f, 1.0f)},
        {.position = glm::vec3(0.5f, 0.5f, 1.0f)},   {.position = glm::vec3(-0.5f, 0.5f, 1.0f)},

    };

    const std::vector<uint16_t> indices = {// front
                                           0, 1, 2, 2, 3, 0,
                                           // right
                                           1, 5, 6, 6, 2, 1,
                                           // back
                                           7, 6, 5, 5, 4, 7,
                                           // left
                                           4, 0, 3, 3, 7, 4,
                                           // bottom
                                           4, 5, 1, 1, 0, 4,
                                           // top
                                           3, 2, 6, 6, 7, 3};
};

class WorldRenderer : public Renderer
{
public:
    WorldRenderer(Device* device, World* world, SwapChain* swapChain, Window* window)
        : Renderer(device, swapChain, window), m_World(world)
    {
        m_World->setCamera(std::move(m_Camera));
    }

    void render();

private:
    void drawWorld(VkCommandBuffer commandBuffer);

    World* m_World;
};

}; // namespace vrender
