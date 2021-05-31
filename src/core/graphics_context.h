#pragma once

#include "app/vrender.h"
#include "app/window.h"
#include "core/device.h"
#include "core/renderer.h"
#include "core/swap_chain.h"
#include "utils/noncopyable.h"

#include <memory>

namespace vrender
{
class GraphicsContext : private NonCopyable
{

public:
    GraphicsContext()
        : m_Window(std::make_unique<Window>(VRender::appInfo().title)),
          m_Device(std::make_unique<Device>(VRender::appInfo(), m_Window.get())),
          m_SwapChain(std::make_unique<SwapChain>(m_Device.get(), m_Window.get())),
          m_Renderer(std::make_unique<Renderer>(m_Device.get(), m_SwapChain.get(), m_Window.get()))
    {
    }
    static GraphicsContext& get()
    {
        static GraphicsContext context;
        return context;
    }

    inline Device* device() const { return m_Device.get(); }
    inline Renderer* renderer() const { return m_Renderer.get(); }
    inline Window* window() const { return m_Window.get(); }
    inline SwapChain* swapChain() const { return m_SwapChain.get(); }

protected:
    static bool create();

private:
    static GraphicsContext* m_Context;

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Device> m_Device;
    std::unique_ptr<SwapChain> m_SwapChain;
    std::unique_ptr<Renderer> m_Renderer;
};
}; // namespace vrender
