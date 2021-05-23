#pragma once

#include "app/window.h"
#include "core/device.h"
#include "core/pipeline.h"
#include "core/renderer.h"

#include "events/key_events.h"
#include "utils/log.h"

namespace vrender
{

class TestClass : public EventHandler
{
    virtual void handle(const Event& event) override
    {
        const KeyPressedEvent& e = reinterpret_cast<const KeyPressedEvent&>(event);
        V_LOG_INFO("Keycode: {}", e.getKeyCode());
    }
};

struct AppInfo
{
    std::string title;
    uint32_t apiMajor;
    uint32_t apiMinor;
};

class VRender
{
public:
    VRender();
    ~VRender();

    VRender(const VRender& vrender) = delete;
    VRender& operator=(const VRender& vrender) = delete;

    int run();

private:
    const AppInfo m_AppInfo = {"VRender", 1, 2};

    Window m_Window;
    Device m_Device;
    SwapChain m_SwapChain;
    Renderer m_Renderer;

    TestClass testClass;
};
}; // namespace vrender
