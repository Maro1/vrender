#pragma once

#include "app/window.h"
#include "core/device.h"
#include "core/pipeline.h"
#include "core/renderer.h"

namespace vrender
{
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
};
}; // namespace vrender
