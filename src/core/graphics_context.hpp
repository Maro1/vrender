#pragma once

#include "app/window.hpp"
#include "core/memory/memory_allocator.hpp"
#include "core/rendering/renderer.hpp"
#include "core/vulkan/device.hpp"
#include "core/vulkan/swap_chain.hpp"
#include "utils/noncopyable.hpp"

#include <memory>

namespace vrender
{

class GraphicsContext : private NonCopyable
{

public:
    static GraphicsContext& get()
    {
        static GraphicsContext context;
        return context;
    }

    void init(const AppInfo& appInfo);

    inline Device* device() const { return m_Device.get(); }
    inline Renderer* renderer() const { return m_Renderer.get(); }
    inline Window* window() const { return m_Window.get(); }
    inline SwapChain* swapChain() const { return m_SwapChain.get(); }
    inline DeviceMemoryAllocator* deviceMemoryAllocator() const { return m_MemoryAllocator.get(); }
    inline Scene* world() const { return m_World.get(); }

protected:
    static bool create();

private:
    GraphicsContext() {}
    static GraphicsContext* m_Context;

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Device> m_Device;
    std::unique_ptr<SwapChain> m_SwapChain;
    std::unique_ptr<DeviceMemoryAllocator> m_MemoryAllocator;
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<Scene> m_World;
};
}; // namespace vrender
