#pragma once

#include "scene/scene.hpp"

namespace vrender
{
class RenderSystem : private NonCopyable
{
public:
    RenderSystem();
    ~RenderSystem();

private:
    std::unique_ptr<Scene> m_Scene;
};

} // namespace vrender