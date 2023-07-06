#pragma once

#include "app/app.hpp"
#include "graphics_context.hpp"

#include <memory>
#include <utility>

namespace vrender
{
class Engine
{
public:
    Engine();
    ~Engine();

    static Engine& get()
    {
        static Engine engine;
        return engine;
    }

    int run();

    void setApp(std::unique_ptr<App> app);

    inline Scene* world() const { return GraphicsContext::get().world(); }

private:
    inline Device* device() const { return GraphicsContext::get().device(); }
    inline Renderer* renderer() const { return GraphicsContext::get().renderer(); }
    inline Window* window() const { return GraphicsContext::get().window(); }
    inline SwapChain* swapChain() const { return GraphicsContext::get().swapChain(); }
    inline DeviceMemoryAllocator* deviceMemoryAllocator() const
    {
        return GraphicsContext::get().deviceMemoryAllocator();
    }

    std::unique_ptr<App> m_App;
    double m_LastTime;
};
} // namespace vrender