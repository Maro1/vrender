#include "vrender.h"

#include "utils/log.h"

namespace vrender
{
VRender::VRender()
    : m_Window(m_AppInfo.title), m_Device(m_AppInfo, m_Window), m_SwapChain(&m_Device, &m_Window),
      m_Renderer(&m_Device, &m_SwapChain, &m_Window)
{
    m_Window.registerHandler(&m_Renderer.cameraController(),
                             {EventType::KeyPress, EventType::KeyRelease, EventType::MouseMove,
                              EventType::MouseButtonPress, EventType::MouseButtonRelease, EventType::InputState});
}

VRender::~VRender() {}

int VRender::run()
{
    V_LOG_LEVEL(V_LOG_LEVEL_DEBUG);
    V_LOG_INFO("Started VRender application.");
    while (!m_Window.shouldClose())
    {
        glfwPollEvents();
        m_Window.update();
        VkCommandBuffer commandBuffer = m_Renderer.beginFrame();
        m_Renderer.beginRenderPass();
        m_Renderer.pipeline().bind(commandBuffer);
        m_Renderer.draw(commandBuffer);
        m_Renderer.endRenderPass();
        m_Renderer.endFrame();
    }
    vkDeviceWaitIdle(m_Device.device());
    return 0;
}
}; // namespace vrender
