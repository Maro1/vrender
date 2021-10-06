#include "vrender.h"

#include "core/graphics_context.h"
#include "scene/model/mesh.h"
#include "utils/log.h"

const std::vector<vrender::Vertex> vertices = {
    {.position = glm::vec3(-0.5f, -0.5f, 0.0f)}, {.position = glm::vec3(0.5f, -0.5f, 0.0f)},
    {.position = glm::vec3(0.5f, 0.5f, 0.0f)},   {.position = glm::vec3(-0.5f, 0.5f, 0.0f)},

    {.position = glm::vec3(-0.5f, -0.5f, 1.0f)}, {.position = glm::vec3(0.5f, -0.5f, 1.0f)},
    {.position = glm::vec3(0.5f, 0.5f, 1.0f)},   {.position = glm::vec3(-0.5f, 0.5f, 1.0f)},

};

const std::vector<uint16_t> indices = { // front
    0, 1, 2, 2, 3, 0,
    // right
    1, 5, 6, 6, 2, 1,
    // back
    7, 6, 5, 5, 4, 7,
    // left
    4, 0, 3, 3, 7, 4,
    // bottom
    4, 5, 1, 1, 0, 4,
    // top
    3, 2, 6, 6, 7, 3};

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

    Entity* entity = GraphicsContext::get().world()->createEntity();
    Mesh mesh(GraphicsContext::get().device(), GraphicsContext::get().world(), vertices, indices);

    entity->addComponent<Mesh>(GraphicsContext::get().device(), GraphicsContext::get().world(), vertices, indices);

    while (!GraphicsContext::get().window()->shouldClose())
    {
        glfwPollEvents();
        GraphicsContext::get().window()->update();
        WorldRenderer* renderer = static_cast<WorldRenderer*>(GraphicsContext::get().renderer());
        renderer->render();

        // VkCommandBuffer commandBuffer = renderer->beginFrame();
        // renderer->beginRenderPass();
        // renderer->pipeline().bind(commandBuffer);
        // renderer->draw(commandBuffer);
        // renderer->endRenderPass();
        // renderer->endFrame();
    }
    vkDeviceWaitIdle(GraphicsContext::get().device()->device());
    return 0;
}
}; // namespace vrender
