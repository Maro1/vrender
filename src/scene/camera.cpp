#include "camera.h"

#include "app/events/key_events.h"
#include "app/events/mouse_events.h"

namespace vrender
{

Camera::~Camera() {}

void Camera::rotate(const glm::quat& angle)
{
    m_Rotation *= angle;
}

void Camera::translate(const glm::vec3& translation)
{
    m_Position += translation;
}

glm::mat4 Camera::view() const
{
    glm::mat4 rot = glm::toMat4(m_Rotation);
    return glm::translate(rot, -m_Position);
}

glm::mat4 Camera::projection() const
{
    return glm::perspective(glm::radians(m_Fovy), m_AspectRatio, m_ZNear, m_ZFar);
}

void CameraController::handle(const Event& event)
{
    switch (event.type())
    {
    case EventType::KeyPress: {
        const KeyPressedEvent& e = reinterpret_cast<const KeyPressedEvent&>(event);
        keyPressed(e.getKeyCode());
        break;
    }
    case EventType::KeyRelease: {
        const KeyReleasedEvent& e = reinterpret_cast<const KeyReleasedEvent&>(event);
        keyReleased(e.getKeyCode());
        break;
    }
    case EventType::MouseButtonPress: {
        const MousePressedEvent& e = reinterpret_cast<const MousePressedEvent&>(event);
        mousePress(e.button(), e.mods(), e.x(), e.y());
        break;
    }
    case EventType::MouseButtonRelease: {
        const MouseReleasedEvent& e = reinterpret_cast<const MouseReleasedEvent&>(event);
        mouseRelease(e.button(), e.x(), e.y());
        break;
    }
    case EventType::MouseMove: {
        const MouseMovedEvent& e = reinterpret_cast<const MouseMovedEvent&>(event);
        mouseMoved(e.x(), e.y());
        break;
    }
    case EventType::InputState: {
        const InputStateEvent& e = reinterpret_cast<const InputStateEvent&>(event);
        update(e.keyStates(), e.deltaTime());
    }
    default:
        break;
    }
}
}; // namespace vrender
