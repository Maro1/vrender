#pragma once

#include "component.h"

#include <bitset>
#include <memory>
#include <queue>

namespace vrender
{

class World;

class Entity
{
public:
    Entity(World* world);

    ~Entity();

    template <typename T> bool hasComponent() const { return (m_ComponentSet & ComponentTypeIDMap::getID<T>()) != 0; }

    template <typename T> void addComponent()
    {
        m_ComponentSet |= ComponentTypeIDMap::getID<T>();
        addComponent(std::make_unique<T>());
    }

    template <typename T, typename... Args> void addComponent(Args&&... args)
    {
        m_ComponentSet |= ComponentTypeIDMap::getID<T>();
        addComponent(std::make_unique<T>(std::forward<Args>(args)...));
    }

    template <typename T> T* getComponent()
    {
        if (!hasComponent<T>())
        {
            return nullptr;
        }
        for (const std::unique_ptr<Component>& component : m_Components)
        {
            T* casted = dynamic_cast<T*>(component.get());
            if (casted)
            {
                return casted;
            }
        }
        return nullptr;
    }

    inline std::uint64_t ID() const { return m_ID; }

    inline World* world() const { return m_World; }

private:
    void addComponent(std::unique_ptr<Component>&& component);

    uint64_t m_ComponentSet;
    std::vector<std::unique_ptr<Component>> m_Components;
    std::uint64_t m_ID;

    World* m_World;

    static std::uint64_t m_NumEntities;
    static std::queue<uint64_t> m_FreeIDs;
};

} // namespace vrender