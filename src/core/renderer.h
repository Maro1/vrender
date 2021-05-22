#pragma once

#include "core/buffer.h"
#include "core/device.h"
#include "core/pipeline.h"
#include "core/swap_chain.h"

#include "scene/camera.h"

#include <vector>

namespace vrender
{

// TODO: Move somewhere else?
struct MVP
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class Renderer
{
public:
    Renderer(Device* device, SwapChain* swapChain, Window* window);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    VkCommandBuffer beginFrame();
    VkResult endFrame();
    void beginRenderPass();
    void endRenderPass();
    void draw(const VkCommandBuffer& commandBuffer); // TODO: MOVE TO SCENE GRAPH

    inline Pipeline& pipeline() { return m_Pipeline; }

private:
    void init();

    VkResult createCommandBuffers();
    void createMVPBuffers();
    void updateMVP();
    void populateDescriptors();

    Device* m_Device;
    SwapChain* m_SwapChain;
    Window* m_Window;

    DescriptorPool m_DescriptorPool;
    Pipeline m_Pipeline;
    Camera m_Camera;

    std::vector<VkFence> m_InFlightFences;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    uint32_t m_CurrentFrame = 0;
    uint32_t m_CurrentImage;

    std::vector<VertexBuffer*> m_VertexBuffers;
    std::vector<Buffer*> m_MVPBuffers;

    std::vector<VkSemaphore> m_RenderFinishedSemaphores;

    const std::vector<Vertex> vertices = {
        {.position = glm::vec3(-0.5f, -0.5f, 0.0f)}, {.position = glm::vec3(0.5f, -0.5f, 0.0f)},
        {.position = glm::vec3(0.5f, 0.5f, 0.0f)},   {.position = glm::vec3(-0.5f, 0.5f, 0.0f)},

        {.position = glm::vec3(-0.5f, -0.5f, 1.0f)}, {.position = glm::vec3(0.5f, -0.5f, 1.0f)},
        {.position = glm::vec3(0.5f, 0.5f, 1.0f)},   {.position = glm::vec3(-0.5f, 0.5f, 1.0f)},

    };

    const std::vector<uint16_t> indices = {
        // front
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
}; // namespace vrender
