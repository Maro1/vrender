#include "arcball_camera.h"

#include "GLFW/glfw3.h"
#include "glm/fwd.hpp"

namespace vrender
{

ArcballCameraController::ArcballCameraController(Camera* camera) : CameraController(camera) {}

void ArcballCameraController::mousePress(int button, int mods, int x, int y)
{
    if (mods == 4)
    {
    }
}

void ArcballCameraController::keyPressed(int key)
{
    switch (key)
    {
    case 25: {
        break;
    }
    case 39: {
        break;
    }
    case 38: {
        break;
    }
    case 40: {
        break;
    }
    }
}

void ArcballCameraController::update(std::bitset<NUM_KEYBOARD_KEYS> keyStates)
{
    int x = 1;
    if (keyStates[87])
        m_Camera->translate(glm::vec3(0.0f, 0.0f, -0.05f));
    if (keyStates[83])
        m_Camera->translate(glm::vec3(0.0f, 0.0f, 0.05f));
    if (keyStates[65])
        m_Camera->translate(glm::vec3(-0.05f, 0.0f, 0.0f));
    if (keyStates[68])
        m_Camera->translate(glm::vec3(0.05f, 0.0f, 0.0f));
}

void ArcballCameraController::mouseMoved(int x, int y) {}
}; // namespace vrender

