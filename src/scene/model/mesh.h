
#pragma once

#include "core/buffer.h"
#include "core/pipeline.h"
#include "core/uniform.h"

#include "ecs/component.h"
#include "ecs/world.h"

namespace vrender
{

// TODO: FIND BETTER SOLUTION FOR THIS
constexpr uint32_t FRAME_OVERLAP = 2;

// TODO: Move somewhere else?
struct MVP
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class Mesh : public Component
{
public:
    Mesh(Device* device, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices,
         World* world = nullptr);
    ~Mesh();

    void start() override {}
    void update() override;

    void draw(const VkCommandBuffer& commandBuffer, const Pipeline& pipeline, uint32_t frameIndex) const;

private:
    VertexBuffer m_VertexBuffer;
    UniformHandler m_UniformHandlers[FRAME_OVERLAP];

    DescriptorSetAllocator m_DescriptorAllocator;
    DescriptorPool m_DescriptorPool;

    uint32_t m_CurrentImage = 0;
};
}; // namespace vrender
