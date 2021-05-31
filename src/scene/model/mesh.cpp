#include "mesh.h"

namespace vrender
{

// NOTE: Careful with passing vertices and indices like this to vertex buffer, who deletes?
Mesh::Mesh(Device* device, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
    : m_VertexBuffer(device, &vertices, &indices)
{
}

Mesh::~Mesh() {}

void Mesh::draw(const VkCommandBuffer& commandBuffer)
{
    m_VertexBuffer.bind(commandBuffer);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_VertexBuffer.indices()->size()), 1, 0, 0, 0);
}

}; // namespace vrender
