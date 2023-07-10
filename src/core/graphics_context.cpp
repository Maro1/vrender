#include "graphics_context.hpp"
#include "app/app.hpp"
#include "utils/log.hpp"

#include <glslang/Public/ShaderLang.h>

namespace vrender
{
void GraphicsContext::init(const AppInfo& appInfo)
{
    if (!glslang::InitializeProcess())
    {
        V_LOG_ERROR("Failed to initialize glslang!");
    }
    m_Window = std::make_unique<Window>(appInfo.title);
    m_Device = std::make_unique<Device>(appInfo, m_Window.get());
    m_MemoryAllocator = std::make_unique<DeviceMemoryAllocator>(device(), device()->memorySize());
    m_SwapChain = std::make_unique<SwapChain>(m_Device.get(), m_Window.get());
    m_World = std::make_unique<Scene>();
    m_Renderer = std::make_unique<Renderer>(m_Device.get(), m_SwapChain.get(), m_Window.get());
}
} // namespace vrender