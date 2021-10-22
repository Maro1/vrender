#include "mesh.hpp"

#include "core/graphics_context.hpp"

#include "assimp/Importer.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace vrender
{

// NOTE: Careful with passing vertices and indices like this to vertex buffer, who deletes?
Mesh::Mesh(Device* device, std::vector<Vertex> vertices, std::vector<uint16_t> indices)
    : m_VertexBuffer(device, vertices, indices), m_UniformHandlers{UniformHandler(sizeof(MVP)),
                                                                   UniformHandler(sizeof(MVP))},
      m_DescriptorAllocator(device), m_DescriptorPool(device, &m_DescriptorAllocator, FRAME_OVERLAP)
{
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

Mesh::Mesh(Device* device, const std::string& filepath)
    : Mesh(device, Mesh::loadFromFile(filepath).first, Mesh::loadFromFile(filepath).second)
{
}

Mesh::~Mesh() {}

void Mesh::update()
{
    if (!getEntity()->hasComponent<Transform>())
    {
        return;
    }
    Transform* transform = getEntity()->getComponent<Transform>();

    MVP mvp;

    mvp.model = glm::mat4(1.0f);
    mvp.model = glm::translate(mvp.model, transform->position);
    mvp.model = glm::scale(mvp.model, transform->scale);

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
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_VertexBuffer.indices().size()), 1, 0, 0, 0);
}

std::pair<std::vector<Vertex>, std::vector<uint16_t>> Mesh::loadFromFile(const std::string& filepath)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                                           aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    if (!scene)
    {
        // TODO: Do something
    }

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    for (uint32_t i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];

        for (uint32_t j = 0; j < mesh->mNumVertices; j++)
        {
            vertices.push_back({glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z)});
        }

        for (uint32_t j = 0; j < mesh->mNumFaces; j++)
        {
            for (uint32_t k = 0; k < mesh->mFaces[j].mNumIndices; k++)
            {
                indices.push_back(mesh->mFaces[j].mIndices[k]);
            }
        }
    }

    return std::pair<std::vector<Vertex>, std::vector<uint16_t>>(vertices, indices);
}

}; // namespace vrender
