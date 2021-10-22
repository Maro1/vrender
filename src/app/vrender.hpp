#pragma once

#include "app/window.hpp"
#include "core/renderer.hpp"
#include "core/vulkan/device.hpp"
#include "core/vulkan/pipeline.hpp"

#include "events/key_events.hpp"
#include "utils/log.hpp"
#include "utils/noncopyable.hpp"

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
