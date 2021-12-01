#include "vrender.hpp"

#include "app/app.hpp"
#include "core/graphics_context.hpp"
#include "core/rendering/render_system.hpp"
#include "scene/model/mesh.hpp"
#include "utils/log.hpp"

namespace vrender
{
VRender::VRender() : App(sAppInfo)
{
    // TODO: Make CameraController a system?
    GraphicsContext::get().window()->registerHandler(GraphicsContext::get().world()->cameraController(),
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
        Entity* entity = world()->createEntity();

        entity->addComponent<Mesh>(device(), "../assets/models/cube.obj");
        entity->addComponent<Transform>();

        entity->getComponent<Transform>()->position = glm::vec3(i * 1.0f, 0.0f, 0.0f);
        entity->getComponent<Transform>()->scale = glm::vec3(0.1f);
    }

    world()->addSystem<MeshRenderSystem>(device(), swapChain(), window(), world());

    while (!GraphicsContext::get().window()->shouldClose())
    {
        glfwPollEvents();
        window()->update();
        world()->update();
    }
    vkDeviceWaitIdle(device()->device());
    return 0;
}
}; // namespace vrender
