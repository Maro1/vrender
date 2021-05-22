#include "camera.h"

namespace vrender
{

Camera::~Camera() {}

void Camera::rotate(const glm::quat& angle)
{
    m_Rotation *= angle;
}

glm::mat4 Camera::view() const
{
    glm::mat4 rot = glm::toMat4(m_Rotation);
    return glm::translate(rot, m_Position);
}

glm::mat4 Camera::projection() const
{
    return glm::perspective(glm::radians(m_Fovy), m_AspectRatio, m_ZNear, m_ZFar);
}

}; // namespace vrender
