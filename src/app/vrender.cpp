#include "vrender.hpp"

#include "core/graphics_context.hpp"
#include "scene/model/mesh.hpp"
#include "utils/log.hpp"

namespace vrender
{
VRender::VRender()
{
    GraphicsContext::get().window()->registerHandler(&GraphicsContext::get().renderer()->cameraController(),
                                                     {EventType::KeyPress, EventType::KeyRelease, EventType::MouseMove,
                                                      EventType::MouseButtonPress, EventType::MouseButtonRelease,
                                                      EventType::InputState});
}

VRender::~VRender() {}

int VRender::run()
{
    V_LOG_LEVEL(V_LOG_LEVEL_DEBUG);
    V_LOG_INFO("Started VRender application.");

    for (uint32_t i = 0; i < 10; i++)
    {
        Entity* entity = GraphicsContext::get().world()->createEntity();

        entity->addComponent<Mesh>(GraphicsContext::get().device(), "../assets/models/cube.obj");
        entity->addComponent<Transform>();

        entity->getComponent<Transform>()->position = glm::vec3(i * 1.0f, 0.0f, 0.0f);
        entity->getComponent<Transform>()->scale = glm::vec3(0.1f);
    }

    while (!GraphicsContext::get().window()->shouldClose())
    {
        glfwPollEvents();
        GraphicsContext::get().window()->update();
        WorldRenderer* renderer = static_cast<WorldRenderer*>(GraphicsContext::get().renderer());
        renderer->render();
    }
    vkDeviceWaitIdle(GraphicsContext::get().device()->device());
    return 0;
}
}; // namespace vrender
