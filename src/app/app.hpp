#pragma once

#include "app/window.hpp"

#include "scene/scene.hpp"

#include "utils/noncopyable.hpp"

#include <memory>
#include <utility>

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
    friend class Engine;

public:
    App(const AppInfo& appInfo) : m_AppInfo(appInfo) {}
    virtual ~App() = default;

    virtual void init() = 0;
    virtual void update(double deltaTime) = 0;
    virtual void terminate() = 0;

    inline AppInfo info() const { return m_AppInfo; }

protected:

    Scene* world() const;

private:
    AppInfo m_AppInfo;
    bool started = false;
};
} // namespace vrender