#pragma once

#include <set>
#include <unordered_set>
#include <vector>

#include "ecs/component.hpp"
#include "ecs/entity.hpp"

#include "scene/camera/camera.hpp"

namespace vrender
{

class Mesh;

class Scene
{
public:
    ~Scene();

    Entity* createEntity();
    void removeEntity(Entity* entity);

    inline const std::unordered_set<Entity*>& entities() const { return m_Entities; }

    inline Camera* camera() const { return m_Camera.get(); }
    void setCamera(std::unique_ptr<Camera> camera) { m_Camera = std::move(camera); }

private:
    std::unordered_set<Entity*> m_Entities;
    std::unique_ptr<Camera> m_Camera;
};

} // namespace vrender