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
    Entity* entity = world()->createEntity();

    entity->addComponent<Mesh>("../assets/models/stool.obj");
    entity->addComponent<Transform>();

    entity->getComponent<Transform>()->position = glm::vec3(0.0f, 0.0f, 0.0f);
    entity->getComponent<Transform>()->scale = glm::vec3(1.1f);

    world()->addSystem<MeshRenderSystem>();

    Texture texture("../assets/models/Stool_Albedo.png");
}

void VRender::update(double deltaTime) {
    for (auto e : world()->entities())
    {
        if (e->hasComponent<Transform>()) {
            //e->getComponent<Transform>()->rotation = glm::rotate(e->getComponent<Transform>()->rotation, glm::vec3(10 * deltaTime, 0, 0));
        }
    }
}

void VRender::terminate() {}

}; // namespace vrender
