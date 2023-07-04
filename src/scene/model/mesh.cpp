#include "mesh.hpp"

#include "core/graphics_context.hpp"

#include "assimp/Importer.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace vrender
{

// NOTE: Careful with passing vertices and indices like this to vertex buffer, who deletes?
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices)
    : m_VertexBuffer(GraphicsContext::get().device(), vertices, indices)
{
}

Mesh::Mesh(const std::string& filepath) : Mesh(Mesh::loadFromFile(filepath).first, Mesh::loadFromFile(filepath).second)
{
}

Mesh::~Mesh() {}

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
            Vertex vertex;
            vertex.position = glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
            vertex.texCoord = glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);

            vertices.push_back(vertex);
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
