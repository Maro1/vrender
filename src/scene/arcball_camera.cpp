#include "arcball_camera.h"

#include "GLFW/glfw3.h"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "glm/gtx/transform.hpp"
#include "utils/log.h"

namespace vrender
{

ArcballCameraController::ArcballCameraController(Camera* camera) : CameraController(camera) {}

void ArcballCameraController::mousePress(int button, int mods, int x, int y)
{
    if (button == 0)
    {
        m_PrevPos = glm::vec2(x, y);
        m_AltMousePressed = true;
    }
}

void ArcballCameraController::mouseRelease(int button, int x, int y)
{
    if (button == 0)
    {
        m_AltMousePressed = false;
    }
}

void ArcballCameraController::keyPressed(int key) {}

void ArcballCameraController::update(std::bitset<NUM_KEYBOARD_KEYS> keyStates, float deltaTime)
{
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f) * m_Camera->rotation();
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f) * m_Camera->rotation();

    if (keyStates[87])
        m_Camera->translate(forward * m_MoveSpeed * deltaTime);
    if (keyStates[83])
        m_Camera->translate(-forward * m_MoveSpeed * deltaTime);
    if (keyStates[65])
        m_Camera->translate(-right * m_MoveSpeed * deltaTime);
    if (keyStates[68])
        m_Camera->translate(right * m_MoveSpeed * deltaTime);
}

void ArcballCameraController::mouseMoved(int x, int y)
{
    if (m_AltMousePressed)
    {
        float deltaX = (float)x - m_PrevPos.x;
        float deltaY = (float)y - m_PrevPos.y;

        m_Camera->setPosition(tumble(deltaX * -m_RotateSpeed, deltaY * -m_RotateSpeed));

        m_Camera->setRotation(m_Camera->rotation() *
                              glm::angleAxis(deltaX * m_RotateSpeed, glm::vec3(0.0f, 1.0f, 0.0f)));

        m_Camera->setRotation(
            m_Camera->rotation() *
            glm::angleAxis(deltaY * m_RotateSpeed, glm::vec3(1.0f, 0.0f, 0.0f) * m_Camera->rotation()));

        // TODO: Remove and always update from initial position
        m_PrevPos = glm::vec2(x, y);
    }
}

glm::vec3 ArcballCameraController::tumble(float deltaX, float deltaY)
{
    // Translate pos to target to origin
    glm::vec3 originPosToTarget = m_Camera->position() - m_Target;

    // Axis of rotation
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f) * m_Camera->rotation();

    glm::vec3 newPos = (1 - glm::cos(deltaX)) * (glm::dot(originPosToTarget, up)) * up +
                       glm::cos(deltaX) * originPosToTarget + glm::sin(deltaX) * glm::cross(up, originPosToTarget);

    newPos = (1 - glm::cos(deltaY)) * (glm::dot(newPos, right)) * right + glm::cos(deltaY) * newPos +
             glm::sin(deltaY) * glm::cross(right, newPos);

    // Translate back
    newPos += m_Target;

    return newPos;
}
}; // namespace vrender

