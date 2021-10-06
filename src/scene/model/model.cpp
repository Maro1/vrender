#include "model.h"

namespace vrender
{

// Model::Model(const std::vector<Mesh>& meshes) : m_Meshes(meshes) {}

Model::~Model() {}

void Model::draw(const VkCommandBuffer& commandBuffer)
{
    for (auto& mesh : m_Meshes)
    {
        //mesh.draw(commandBuffer);
    }
}
}; // namespace vrender
