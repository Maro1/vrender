#pragma once

#include "core/vulkan/buffer.hpp"
#include "core/vulkan/pipeline.hpp"
#include "core/vulkan/uniform.hpp"

#include "ecs/component.hpp"
#include "scene/scene.hpp"

namespace vrender
{

class Mesh : public Component
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices);
    Mesh(const std::string& filepath);
    ~Mesh();

    inline VertexBuffer* vertexBuffer() { return &m_VertexBuffer; }

    static std::pair<std::vector<Vertex>, std::vector<uint16_t>> loadFromFile(const std::string& filepath);

private:
    VertexBuffer m_VertexBuffer;

    uint32_t m_CurrentImage = 0;
};
}; // namespace vrender
