#pragma once

#include "app/app.hpp"

#include "events/key_events.hpp"
#include "utils/log.hpp"
#include "utils/noncopyable.hpp"

namespace vrender
{

class VRender : public App
{
public:
    VRender();
    ~VRender();

    virtual void init() override;
    virtual void update(double deltaTime) override;
    virtual void terminate() override;

private:
    inline static const AppInfo sAppInfo = {"VRender", 1, 2};
};
}; // namespace vrender
