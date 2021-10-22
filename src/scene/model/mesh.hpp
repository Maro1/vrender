
#pragma once

#include "core/vulkan/buffer.hpp"
#include "core/vulkan/pipeline.hpp"
#include "core/vulkan/uniform.hpp"

#include "ecs/component.hpp"
#include "scene/scene.hpp"

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
    Mesh(Device* device, std::vector<Vertex> vertices, std::vector<uint16_t> indices);
    Mesh(Device* device, const std::string& filepath);
    ~Mesh();

    void start() override {}
    void update() override;

    void draw(const VkCommandBuffer& commandBuffer, const Pipeline& pipeline, uint32_t frameIndex) const;

    static std::pair<std::vector<Vertex>, std::vector<uint16_t>> loadFromFile(const std::string& filepath);

private:
    VertexBuffer m_VertexBuffer;
    UniformHandler m_UniformHandlers[FRAME_OVERLAP];

    DescriptorSetAllocator m_DescriptorAllocator;
    DescriptorPool m_DescriptorPool;

    uint32_t m_CurrentImage = 0;
};
}; // namespace vrender
