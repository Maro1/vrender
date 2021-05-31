
#pragma once

#include "core/buffer.h"

namespace vrender
{
class Mesh
{
public:
    Mesh(Device* device, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
    ~Mesh();

    void draw(const VkCommandBuffer& commandBuffer);

private:
    VertexBuffer m_VertexBuffer;
};
}; // namespace vrender
