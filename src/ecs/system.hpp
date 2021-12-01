#pragma once

#include "utils/noncopyable.hpp"

#include "ecs/entity.hpp"
#include <unordered_set>

class Scene;

namespace vrender
{
class System : private NonCopyable
{
public:
    System(Scene* scene) : m_Scene(scene) {}

    virtual void start() {}
    virtual void update() {}

protected:
    const std::unordered_set<Entity*>& entities() const;

    Scene* m_Scene;
};
} // namespace vrender