#pragma once

#include "app/window.h"
#include "core/device.h"
#include "core/pipeline.h"
#include "core/renderer.h"

#include "events/key_events.h"
#include "utils/log.h"
#include "utils/noncopyable.h"

namespace vrender
{

struct AppInfo
{
    std::string title;
    uint32_t apiMajor;
    uint32_t apiMinor;
};

class VRender : private NonCopyable
{
public:
    VRender();
    ~VRender();

    int run();
    static const AppInfo& appInfo() { return sAppInfo; }

private:
    inline static const AppInfo sAppInfo = {"VRender", 1, 2};
};
}; // namespace vrender
