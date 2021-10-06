
#pragma once

#include "core/buffer.h"
#include "ecs/component.h"
#include "ecs/world.h"

namespace vrender
{
class Mesh : public Component
{
public:
    Mesh(Device* device, World* world, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
    ~Mesh();

    void start() override {}
    void update() override;

    void draw(const VkCommandBuffer& commandBuffer) const;

private:
    VertexBuffer m_VertexBuffer;
    VkDescriptorSet m_DescriptorSet;
};
}; // namespace vrender
