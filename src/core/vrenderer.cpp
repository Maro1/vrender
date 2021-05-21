#include "vrenderer.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

VRenderer::VRenderer(VDevice* device, VSwapChain* swapChain, VWindow* window, VPipeline* pipeline)
    : m_Device(device), m_SwapChain(swapChain), m_Window(window), m_DescriptorPool(device, swapChain->images().size()),
      m_Pipeline(pipeline)
{
    m_VertexBuffers.push_back(new VVertexBuffer(m_Device, &vertices, &indices));
    createMVPBuffers();
    populateDescriptors();
    init();
}

VRenderer::~VRenderer()
{
    for (auto buffer : m_VertexBuffers)
    {
        delete buffer;
    }
    for (auto buffer : m_MVPBuffers)
    {
        delete buffer;
    }
}

void VRenderer::init()
{
    createCommandBuffers();
}

VkCommandBuffer VRenderer::beginFrame()
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

VkResult VRenderer::endFrame()
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
    m_CurrentFrame = (m_CurrentFrame + 1) % VSwapChain::MAX_FRAMES_IN_FLIGHT;
    return queuePresentResult;
}

void VRenderer::beginRenderPass()
{
    VkCommandBuffer commandBuffer = m_CommandBuffers[m_CurrentFrame];

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_SwapChain->renderPass();
    renderPassInfo.framebuffer = m_SwapChain->framebuffer(m_CurrentImage)->framebuffer();
    renderPassInfo.renderArea = {0, 0};
    renderPassInfo.renderArea.extent = m_SwapChain->extent();

    VkClearValue clearColor = {0.1f, 0.1f, 0.1f, 1.0f};
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

void VRenderer::endRenderPass()
{
    vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);
}

VkResult VRenderer::createCommandBuffers()
{
    m_CommandBuffers.resize(m_SwapChain->framebuffers().size());
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_Device->commandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

    return vkAllocateCommandBuffers(m_Device->device(), &allocInfo, m_CommandBuffers.data());
}

void VRenderer::draw(const VkCommandBuffer& commandBuffer)
{
    m_VertexBuffers[0]->bind(commandBuffer);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->layout(), 0, 1,
                            &m_DescriptorPool.descriptorSets()[m_CurrentFrame], 0, nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_VertexBuffers[0]->indices()->size()), 1, 0, 0, 0);
}

void VRenderer::updateMVP()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    MVP mvp;
    mvp.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mvp.proj = glm::perspective(glm::radians(45.0f), m_SwapChain->extent().width / (float)m_SwapChain->extent().height,
                                0.1f, 10.0f);
    mvp.proj[1][1] *= -1;

    void* data = m_MVPBuffers[m_CurrentImage]->copyData((void*)&mvp, sizeof(mvp));
}

void VRenderer::createMVPBuffers()
{
    VkDeviceSize bufferSize = sizeof(MVP);

    m_MVPBuffers.resize(m_SwapChain->images().size());

    VBufferInfo bufferInfo;
    bufferInfo.size = bufferSize;
    bufferInfo.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    for (unsigned int i = 0; i < m_SwapChain->images().size(); i++)
    {
        m_MVPBuffers[i] = new VBuffer(m_Device, bufferInfo);
    }
}

void VRenderer::populateDescriptors()
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

