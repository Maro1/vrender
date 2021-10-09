#include "mesh.h"

#include <chrono>

namespace vrender
{

// NOTE: Careful with passing vertices and indices like this to vertex buffer, who deletes?
Mesh::Mesh(Device* device, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices, World* world)
    : m_VertexBuffer(device, &vertices, &indices), m_UniformHandlers{UniformHandler(sizeof(MVP)),
                                                                     UniformHandler(sizeof(MVP))},
      m_DescriptorAllocator(device), m_DescriptorPool(device, &m_DescriptorAllocator, FRAME_OVERLAP)
{
    if (world)
    {
        world->addMesh(this);
    }

    for (uint32_t i = 0; i < FRAME_OVERLAP; i++)
    {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = m_UniformHandlers[i].buffer()->buffer();
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

        vkUpdateDescriptorSets(device->device(), 1, &descriptorWrite, 0, nullptr);
    }
}

Mesh::~Mesh() {}

void Mesh::update()
{
    if (!getEntity()->hasComponent<Transform>())
    {
        return;
    }
    Transform* transform = getEntity()->getComponent<Transform>();

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    MVP mvp;

    mvp.model = glm::mat4(1.0f);
    mvp.model = glm::rotate(mvp.model, time * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mvp.model = glm::translate(mvp.model, transform->position);

    mvp.view = getEntity()->world()->camera()->view();
    mvp.proj = getEntity()->world()->camera()->projection();
    mvp.proj[1][1] *= -1;

    void* data = m_UniformHandlers[m_CurrentImage++ % 2].buffer()->copyData((void*)&mvp, sizeof(mvp));
}

void Mesh::draw(const VkCommandBuffer& commandBuffer, const Pipeline& pipeline, uint32_t frameIndex) const
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.layout(), 0, 1,
                            &m_DescriptorPool.descriptorSets()[frameIndex], 0, nullptr);
    m_VertexBuffer.bind(commandBuffer);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_VertexBuffer.indices()->size()), 1, 0, 0, 0);
}

}; // namespace vrender
