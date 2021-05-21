#pragma once

#include "core/vbuffer.h"
#include "core/vdevice.h"
#include "core/vpipeline.h"
#include "core/vswap_chain.h"

#include <vector>

// TODO: Move somewhere else?
struct MVP
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class VRenderer
{
public:
    VRenderer(VDevice* device, VSwapChain* swapChain, VWindow* window);
    ~VRenderer();

    VRenderer(const VRenderer&) = delete;
    VRenderer& operator=(const VRenderer&) = delete;

    VkCommandBuffer beginFrame();
    VkResult endFrame();
    void beginRenderPass();
    void endRenderPass();
    void draw(const VkCommandBuffer& commandBuffer); // TODO: MOVE TO SCENE GRAPH

    inline VPipeline& pipeline()
    {
        return m_Pipeline;
    }

private:
    void init();

    VkResult createCommandBuffers();
    void createMVPBuffers();
    void updateMVP();
    void populateDescriptors();

    VDevice* m_Device;
    VSwapChain* m_SwapChain;
    VWindow* m_Window;

    VDescriptorPool m_DescriptorPool;
    VPipeline m_Pipeline;

    std::vector<VkFence> m_InFlightFences;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    uint32_t m_CurrentFrame = 0;
    uint32_t m_CurrentImage;

    std::vector<VVertexBuffer*> m_VertexBuffers;
    std::vector<VBuffer*> m_MVPBuffers;

    std::vector<VkSemaphore> m_RenderFinishedSemaphores;

    const std::vector<Vertex> vertices = {
        {.position = glm::vec3(-0.5f, -0.5f, 0.0f)},
        {.position = glm::vec3(0.5f, -0.5f, 0.0f)},
        {.position = glm::vec3(0.5f, 0.5f, 0.0f)},
        {.position = glm::vec3(-0.5f, 0.5f, 0.0f)},
    };

    const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};
};
