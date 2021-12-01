#include "system.hpp"

#include "scene/scene.hpp"

namespace vrender
{

const std::unordered_set<Entity*>& System::entities() const
{
    return m_Scene->entities();
}

} // namespace vrender