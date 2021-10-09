#include "entity.h"

std::uint64_t vrender::Entity::m_NumEntities{0};
std::queue<uint64_t> vrender::Entity::m_FreeIDs{};

namespace vrender
{
Entity::Entity(World* world) : m_World(world)
{
    if (!Entity::m_FreeIDs.empty())
    {
        m_ID = Entity::m_FreeIDs.front();
        Entity::m_FreeIDs.pop();
    }
    else
    {
        m_ID = Entity::m_NumEntities++;
    }
}

Entity::~Entity()
{
    Entity::m_FreeIDs.push(m_ID);
}

void Entity::addComponent(std::unique_ptr<Component>&& component)
{
    component->setEntity(this);
    m_Components.emplace_back(std::move(component)).get();
}
} // namespace vrender