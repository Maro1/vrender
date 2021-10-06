#include "world.h"

namespace vrender
{

std::atomic<ComponentType> ComponentTypeIDMap::m_TypeIdCounter{1};

Entity* World::createEntity()
{
    Entity* entity = new Entity(this);
    m_Entities.insert(entity);

    return entity;
}

void World::removeEntity(Entity* entity)
{
    m_Entities.erase(entity);
}

} // namespace vrender