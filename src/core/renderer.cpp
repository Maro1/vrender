#include "renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

namespace vrender
{

Renderer::Renderer(Device* device, SwapChain* swapChain, Window* window)
    : m_Device(device), m_SwapChain(swapChain), m_Window(window), m_DescriptorAllocator(device),
      m_DescriptorPool(device, &m_DescriptorAllocator, swapChain->images().size()),
      m_Pipeline(device, swapChain, m_DescriptorPool.allocator()), m_CameraController(&m_Camera)
{
    std::unique_ptr<VertexBuffer> cube = std::make_unique<VertexBuffer>(m_Device, &vertices, &indices);
    m_VertexBuffers.push_back(std::move(cube));
    createMVPBuffers();
    populateDescriptors();
    init();
    m_Camera.setAspectRatio((float)m_SwapChain->extent().width / (float)m_SwapChain->extent().height);
    m_Camera.setPosition(glm::vec3(0.0f, 0.0f, 4.0f));
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

    updateMVP();

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

void Renderer::draw(const VkCommandBuffer& commandBuffer)
{
    m_VertexBuffers[0]->bind(commandBuffer);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline.layout(), 0, 1,
                            &m_DescriptorPool.descriptorSets()[m_CurrentFrame], 0, nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_VertexBuffers[0]->indices()->size()), 1, 0, 0, 0);
}

void Renderer::updateMVP()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    MVP mvp;

    mvp.model = glm::mat4(1.0f);
    mvp.model = glm::rotate(mvp.model, time * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mvp.model = glm::translate(mvp.model, glm::vec3(0.0f, 0.0f, 0.0f));
    mvp.view = m_Camera.view();
    mvp.proj = m_Camera.projection();
    mvp.proj[1][1] *= -1;

    void* data = m_MVPBuffers[m_CurrentImage]->copyData((void*)&mvp, sizeof(mvp));
}

void Renderer::createMVPBuffers()
{
    VkDeviceSize bufferSize = sizeof(MVP);

    BufferInfo bufferInfo;
    bufferInfo.size = bufferSize;
    bufferInfo.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    for (unsigned int i = 0; i < m_SwapChain->images().size(); i++)
    {
        std::unique_ptr<Buffer> buffer = std::make_unique<Buffer>(bufferInfo);
        m_MVPBuffers.push_back(std::move(buffer));
    }
}

void Renderer::populateDescriptors()
{
    for (unsigned int i = 0; i < m_SwapChain->images().size(); i++)
    {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = m_MVPBuffers[i]->buffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(MVP);

        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_DescriptorPool.descriptorSets()[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(m_Device->device(), 1, &descriptorWrite, 0, nullptr);
    }
}

void WorldRenderer::render()
{
    VkCommandBuffer commandBuffer = beginFrame();
    beginRenderPass();
    pipeline().bind(commandBuffer);
    drawWorld(commandBuffer);
    endRenderPass();
    endFrame();
}

void WorldRenderer::drawWorld(VkCommandBuffer commandBuffer)
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline.layout(), 0, 1,
                            &m_DescriptorPool.descriptorSets()[m_CurrentFrame], 0, nullptr);
    for (const Mesh* mesh : m_World->meshes())
    {
        mesh->draw(commandBuffer);
    }
}

}; // namespace vrender
