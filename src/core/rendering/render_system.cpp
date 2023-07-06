#include "render_system.hpp"

#include "core/graphics_context.hpp"
#include "core/vulkan/descriptor_set.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "scene/model/mesh.hpp"
#include "utils/log.hpp"
#include <vulkan/vulkan_core.h>

namespace vrender
{
MeshRenderSystem::MeshRenderSystem()
    : System(GraphicsContext::get().world()),
      m_Renderer(GraphicsContext::get().device(), GraphicsContext::get().swapChain(), GraphicsContext::get().window()),
      m_DescriptorAllocator(GraphicsContext::get().device(), &m_Renderer.pipeline()),
      m_GlobalUniformHandler(sizeof(GlobalUBO)),
      m_DescriptorPool(&m_DescriptorAllocator, DESCRIPTOR_TYPES, FRAME_OVERLAP),
      m_Texture("../assets/models/Stool_Albedo.png")
{
    for (uint32_t i = 0; i < FRAME_OVERLAP; i++)
    {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = m_GlobalUniformHandler.buffer()->buffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(GlobalUBO);

        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_Texture.imageView();
        imageInfo.sampler = m_Texture.sampler();

        std::array<VkWriteDescriptorSet, 3> descriptorWrites = {};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = m_DescriptorPool.descriptorSets()[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = m_DescriptorPool.descriptorSets()[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &bufferInfo;

        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = m_DescriptorPool.descriptorSets()[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(GraphicsContext::get().device()->device(), 3, descriptorWrites.data(), 0, nullptr);
    }
}
void MeshRenderSystem::start()
{
    m_Scene->camera()->setAspectRatio((float)m_Renderer.swapChain()->extent().width /
                                      (float)m_Renderer.swapChain()->extent().height);
    m_Scene->camera()->setPosition(glm::vec3(0.0f, 0.0f, 4.0f));
}

void MeshRenderSystem::update()
{
    VkCommandBuffer commandBuffer = m_Renderer.beginFrame();
    m_Renderer.beginRenderPass();
    m_Renderer.pipeline().bind(commandBuffer);

    // TODO: Don't do this each frame?
    for (Entity* entity : entities())
    {
        if (entity->hasComponent<Mesh>() &&
            entity->hasComponent<Transform>()) // TODO: Instead of checking for Mesh, create Renderable type check?
        {
            Transform* transform = entity->getComponent<Transform>();
            Mesh* mesh = entity->getComponent<Mesh>();

            glm::mat4 model(1.0f);
            model = glm::translate(model, transform->position);
            model = glm::scale(model, transform->scale);
            model *= glm::toMat4(transform->rotation);

            PushData pushData = {model};

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Renderer.pipeline().layout(), 0,
                                    1, &m_DescriptorPool.descriptorSets()[m_Renderer.currentFrame()], 0, nullptr);

            glm::mat4 proj = m_Scene->camera()->projection();
            glm::mat4 view = m_Scene->camera()->view();
            glm::mat4 projView = m_Scene->camera()->projection() * m_Scene->camera()->view();
            GlobalUBO ubo = {projView};

            m_GlobalUniformHandler.buffer()->copyData((void*)&ubo, sizeof(GlobalUBO));

            vkCmdPushConstants(commandBuffer, m_Renderer.pipeline().layout(), VK_SHADER_STAGE_VERTEX_BIT, 0,
                               sizeof(PushData), &pushData);

            mesh->vertexBuffer()->bind(commandBuffer);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh->vertexBuffer()->indices().size()), 1, 0, 0, 0);
        }
    }

    m_Renderer.endRenderPass();
    m_Renderer.endFrame();
}
} // namespace vrender