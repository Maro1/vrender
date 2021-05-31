#include "vrender.h"

#include "core/graphics_context.h"
#include "utils/log.h"

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
    while (!GraphicsContext::get().window()->shouldClose())
    {
        glfwPollEvents();
        GraphicsContext::get().window()->update();
        Renderer* renderer = GraphicsContext::get().renderer();
        VkCommandBuffer commandBuffer = renderer->beginFrame();
        renderer->beginRenderPass();
        renderer->pipeline().bind(commandBuffer);
        renderer->draw(commandBuffer);
        renderer->endRenderPass();
        renderer->endFrame();
    }
    vkDeviceWaitIdle(GraphicsContext::get().device()->device());
    return 0;
}
}; // namespace vrender
