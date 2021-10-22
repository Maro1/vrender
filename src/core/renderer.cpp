#include "renderer.hpp"

#include "scene/model/mesh.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

namespace vrender
{

Renderer::Renderer(Device* device, SwapChain* swapChain, Window* window)
    : m_Device(device), m_SwapChain(swapChain), m_Window(window), m_Pipeline(device, swapChain),
      m_Camera(std::unique_ptr<Camera>(new Camera())), m_CameraController(m_Camera.get())
{
    init();
    m_Camera->setAspectRatio((float)m_SwapChain->extent().width / (float)m_SwapChain->extent().height);
    m_Camera->setPosition(glm::vec3(0.0f, 0.0f, 4.0f));
}

Renderer::~Renderer() {}

void Renderer::init()
{
    createCommandBuffers();
}

VkCommandBuffer Renderer::beginFrame()
{
    VkResult imageResult = m_SwapChain->aquireNextImage(m_CurrentImage);

    if (imageResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        m_SwapChain->recreate();
        return VK_NULL_HANDLE;
    }

    VkCommandBuffer commandBuffer = m_CommandBuffers[m_CurrentFrame];

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VkResult beginResult = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (beginResult != VK_SUCCESS)
        return VK_NULL_HANDLE;

    return commandBuffer;
}

VkResult Renderer::endFrame()
{
    VkCommandBuffer commandBuffer = m_CommandBuffers[m_CurrentFrame];
    VkResult commandResult = vkEndCommandBuffer(commandBuffer);
    if (commandResult != VK_SUCCESS)
        return commandResult;

    VkResult queuePresentResult = m_SwapChain->submitCommandBuffers(&m_CommandBuffers[m_CurrentFrame], m_CurrentImage);

    if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR ||
        m_Window->framebufferResized())
    {
        m_Window->setFrameBufferResized(false);
        m_SwapChain->recreate();
    }
    m_CurrentFrame = (m_CurrentFrame + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    return queuePresentResult;
}

void Renderer::beginRenderPass()
{
    VkCommandBuffer commandBuffer = m_CommandBuffers[m_CurrentFrame];

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_SwapChain->renderPass();
    renderPassInfo.framebuffer = m_SwapChain->framebuffer(m_CurrentImage);
    renderPassInfo.renderArea = {0, 0};
    renderPassInfo.renderArea.extent = m_SwapChain->extent();

    VkClearValue clearColor = {0.05f, 0.05f, 0.05f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_SwapChain->extent().width);
    viewport.height = static_cast<float>(m_SwapChain->extent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, m_SwapChain->extent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::endRenderPass()
{
    vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);
}

VkResult Renderer::createCommandBuffers()
{
    m_CommandBuffers.resize(m_SwapChain->framebuffers().size());
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_Device->commandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

    return vkAllocateCommandBuffers(m_Device->device(), &allocInfo, m_CommandBuffers.data());
}

void WorldRenderer::render()
{
    // TODO: Replace with world.update() which should update all components and not be called from here
    for (Entity* entity : m_World->entities())
    {
        if (entity->hasComponent<Mesh>())
        {
            Mesh* mesh = entity->getComponent<Mesh>();
            mesh->update();
        }
    }

    VkCommandBuffer commandBuffer = beginFrame();
    beginRenderPass();
    pipeline().bind(commandBuffer);
    drawWorld(commandBuffer);
    endRenderPass();
    endFrame();
}

void WorldRenderer::drawWorld(VkCommandBuffer commandBuffer)
{
    for (Entity* entity : m_World->entities())
    {
        if (entity->hasComponent<Mesh>())
        {
            Mesh* mesh = entity->getComponent<Mesh>();
            mesh->draw(commandBuffer, m_Pipeline, m_CurrentFrame);
        }
    }
}

}; // namespace vrender
