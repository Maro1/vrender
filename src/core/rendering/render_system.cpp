#include "render_system.hpp"

#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "scene/model/mesh.hpp"

namespace vrender
{
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
            //! model *= transform->rotation;

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