#include "mesh.h"

namespace vrender
{

// NOTE: Careful with passing vertices and indices like this to vertex buffer, who deletes?
Mesh::Mesh(Device* device, World* world, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
    : m_VertexBuffer(device, &vertices, &indices)
{
    world->addMesh(this);
}

Mesh::~Mesh() {}

void Mesh::update()
{
    if (!getEntity()->hasComponent<Transform>())
    {
        return;
    }
    Transform* transform = getEntity()->getComponent<Transform>();
}

void Mesh::draw(const VkCommandBuffer& commandBuffer) const
{
    m_VertexBuffer.bind(commandBuffer);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_VertexBuffer.indices()->size()), 1, 0, 0, 0);
}

}; // namespace vrender
