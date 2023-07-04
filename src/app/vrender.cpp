#include "vrender.hpp"

#include "core/engine.hpp"
#include "core/rendering/render_system.hpp"
#include "scene/model/mesh.hpp"

namespace vrender
{
VRender::VRender() : App(sAppInfo) {}

VRender::~VRender() {}

void VRender::init()
{
    // TODO: Make CameraController a system?
    GraphicsContext::get().window()->registerHandler(GraphicsContext::get().world()->cameraController(),
                                                     {EventType::KeyPress, EventType::KeyRelease, EventType::MouseMove,
                                                      EventType::MouseButtonPress, EventType::MouseButtonRelease,
                                                      EventType::InputState});
    for (uint32_t i = 0; i < 10; i++)
    {
        Entity* entity = Engine::get().world()->createEntity();

        entity->addComponent<Mesh>("../assets/models/cube.obj");
        entity->addComponent<Transform>();

        entity->getComponent<Transform>()->position = glm::vec3(i * 1.0f, 0.0f, 0.0f);
        entity->getComponent<Transform>()->scale = glm::vec3(0.1f);
    }

    Engine::get().world()->addSystem<MeshRenderSystem>();

    Texture texture("../assets/texture.jpg");
}

void VRender::update(double deltaTime) {}

void VRender::terminate() {}

}; // namespace vrender
