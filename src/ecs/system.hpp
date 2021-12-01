#pragma once

#include "scene/scene.hpp"
#include "utils/noncopyable.hpp"

namespace vrender
{
class System : private NonCopyable
{
public:
    System(Scene* scene) : m_Scene(scene) {}

    virtual void start() {}
    virtual void update() {}

protected:
    inline const std::unordered_set<Entity*>& entities() const { return m_Scene->entities(); }

    Scene* m_Scene;
};
} // namespace vrender