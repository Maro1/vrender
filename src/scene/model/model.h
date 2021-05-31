#pragma once

#include "mesh.h"

#include <vector>

namespace vrender
{
class Model
{
public:
    // Model(const std::vector<Mesh>& meshes);
    ~Model();

    void draw(const VkCommandBuffer& commandBuffer);

private:
    std::vector<Mesh> m_Meshes;
};
}; // namespace vrender
