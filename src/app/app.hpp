#pragma once

#include "app/window.hpp"

#include "core/graphics_context.hpp"
#include "core/memory/memory_allocator.hpp"
#include "core/rendering/renderer.hpp"
#include "core/vulkan/device.hpp"
#include "core/vulkan/swap_chain.hpp"

#include "utils/noncopyable.hpp"

#include <memory>

namespace vrender
{

struct AppInfo
{
    std::string title;
    uint32_t apiMajor;
    uint32_t apiMinor;
};

class App : private NonCopyable
{
public:
    App(const AppInfo& appInfo) { GraphicsContext::get().init(appInfo); }

    virtual int run() = 0;

protected:
    inline Device* device() const { return GraphicsContext::get().device(); }
    inline Renderer* renderer() const { return GraphicsContext::get().renderer(); }
    inline Window* window() const { return GraphicsContext::get().window(); }
    inline SwapChain* swapChain() const { return GraphicsContext::get().swapChain(); }
    inline DeviceMemoryAllocator* deviceMemoryAllocator() const
    {
        return GraphicsContext::get().deviceMemoryAllocator();
    }
    inline Scene* world() const { return GraphicsContext::get().world(); }

private:
};
} // namespace vrender