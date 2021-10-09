#pragma once

#include <set>
#include <unordered_set>
#include <vector>

#include "component.h"
#include "entity.h"

#include "scene/camera/camera.h"

namespace vrender
{

class Mesh;

class World
{
public:
    Entity* createEntity();
    void removeEntity(Entity* entity);

    inline void addMesh(Mesh* mesh) { m_Meshes.insert(mesh); }
    inline const std::unordered_set<Mesh*>& meshes() const { return m_Meshes; }

    inline Camera* camera() const { return m_Camera.get(); }
    void setCamera(std::unique_ptr<Camera> camera) { m_Camera = std::move(camera); }

private:
    std::unordered_set<Entity*> m_Entities;
    std::unordered_set<Mesh*> m_Meshes;
    std::unique_ptr<Camera> m_Camera;

    // std::unique_ptr<Scene> m_Scene;
};

} // namespace vrender