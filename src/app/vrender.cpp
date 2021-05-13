#include "vrender.h"

#include "utils/vlog.h"

VRender::VRender() : m_Window(m_AppInfo.title), m_Device(m_AppInfo, m_Window), m_SwapChain(m_Window, m_Device), m_Pipeline(m_Device, m_SwapChain), m_Renderer(m_Device, m_SwapChain, m_Window)
{
}

VRender::~VRender()
{
    
}

int VRender::run()
{
    V_LOG_LEVEL(V_LOG_LEVEL_DEBUG);
    V_LOG_INFO("Started VRender application.");
    while (!m_Window.shouldClose()) {
        VkCommandBuffer commandBuffer = m_Renderer.beginFrame();
        m_Renderer.beginRenderPass();
        m_Pipeline.bind(commandBuffer);
        m_Renderer.draw(commandBuffer);
        m_Renderer.endRenderPass();
        m_Renderer.endFrame();
    }
    vkDeviceWaitIdle(m_Device.device());
    return 0;
}


