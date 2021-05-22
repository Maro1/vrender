#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

namespace vrender
{
class Camera
{
public:
    Camera() {}
    Camera(glm::vec3 position, glm::vec3 rotation) : m_Position(position), m_Rotation(rotation) {}
    ~Camera();

    inline void setAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; }
    inline void setPosition(const glm::vec3& position) { m_Position = position; }

    void rotate(const glm::quat& angle);
    glm::mat4 view() const;
    glm::mat4 projection() const;

private:
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::quat m_Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    float m_Fovy = 45.0f;
    float m_ZNear = 1.0f;
    float m_ZFar = 1000.0f;

    float m_AspectRatio = 0.0f;
};
} // namespace vrender
