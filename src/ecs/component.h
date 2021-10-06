#pragma once

#include <atomic>
#include <cstdint>

#include "glm/gtx/quaternion.hpp"
#include "glm/vec3.hpp"

namespace vrender
{

using ComponentType = std::uint64_t;

class Entity;

class ComponentTypeIDMap
{
public:
    template <typename T> static ComponentType getID()
    {
        static const ComponentType id = m_TypeIdCounter;
        m_TypeIdCounter = m_TypeIdCounter << 1;
        return id;
    }

private:
    static std::atomic<ComponentType> m_TypeIdCounter;
};

class Component
{
public:
    virtual void start() = 0;
    virtual void update() = 0;

    inline Entity* getEntity() { return m_Entity; };
    inline void setEntity(Entity* entity) { m_Entity = entity; }

private:
    Entity* m_Entity;
};

struct Transform : public Component
{
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
};

} // namespace vrender