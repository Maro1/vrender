#include "scene.hpp"

namespace vrender
{

std::atomic<ComponentType> ComponentTypeIDMap::m_TypeIdCounter{1};

Scene::~Scene()
{
    for (Entity* entity : m_Entities)
    {
        delete entity;
    }
}

Entity* Scene::createEntity()
{
    Entity* entity = new Entity(this);
    m_Entities.insert(entity);

    return entity;
}

void Scene::removeEntity(Entity* entity)
{
    m_Entities.erase(entity);
    delete entity;
}

} // namespace vrender