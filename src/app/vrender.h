#pragma once

#include "app/vwindow.h"
#include "core/vdevice.h"
#include "core/vpipeline.h"
#include "core/vrenderer.h"

struct VAppInfo
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
    const VAppInfo m_AppInfo = {"VRender", 1, 2};
    
    VWindow m_Window;
    VDevice m_Device;
    VSwapChain m_SwapChain;
    VRenderer m_Renderer;
    VPipeline m_Pipeline;
};
