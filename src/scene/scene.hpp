#pragma once

#include <memory>
#include <set>
#include <unordered_set>
#include <vector>

#include "ecs/component.hpp"
#include "ecs/entity.hpp"

#include "scene/camera/arcball_camera.hpp"

namespace vrender
{

class Mesh;

class Scene
{
public:
    Scene()
        : m_Camera(std::make_unique<Camera>()),
          m_CameraController(std::make_unique<ArcballCameraController>(m_Camera.get()))
    {
    }
    ~Scene();

    Entity* createEntity();
    void removeEntity(Entity* entity);

    inline const std::unordered_set<Entity*>& entities() const { return m_Entities; }

    inline CameraController* cameraController() const { return m_CameraController.get(); }
    inline Camera* camera() const { return m_Camera.get(); }
    void setCamera(std::unique_ptr<Camera> camera) { m_Camera = std::move(camera); }

private:
    std::unordered_set<Entity*> m_Entities;

    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<CameraController> m_CameraController;
};

} // namespace vrender