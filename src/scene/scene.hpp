#pragma once

#include <memory>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ecs/component.hpp"
#include "ecs/entity.hpp"
#include "ecs/system.hpp"

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

    void update();

    Entity* createEntity();
    void removeEntity(Entity* entity);

    template <typename T, typename... Args> System* addSystem(Args&&... args)
    {
        m_Systems.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        m_Systems.back()->start();
        return m_Systems.back().get();
    }

    inline const std::unordered_set<Entity*>& entities() const { return m_Entities; }

    inline CameraController* cameraController() const { return m_CameraController.get(); }
    inline Camera* camera() const { return m_Camera.get(); }
    void setCamera(std::unique_ptr<Camera> camera) { m_Camera = std::move(camera); }

private:
    std::unordered_set<Entity*> m_Entities;
    std::vector<std::unique_ptr<System>> m_Systems;

    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<CameraController> m_CameraController;
};

} // namespace vrender