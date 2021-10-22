#pragma once

#include "app/key_codes.hpp"
#include "camera.hpp"
#include <bitset>

namespace vrender
{

class ArcballCameraController : public CameraController
{
public:
    ArcballCameraController(Camera* camera);

    inline void setMoveSpeed(double moveSpeed) { m_MoveSpeed = moveSpeed; }

    inline double moveSpeed() const { return m_MoveSpeed; }

protected:
    virtual void mouseMoved(int x, int y) override;
    virtual void mousePress(int button, int mods, int x, int y) override;
    virtual void mouseRelease(int button, int x, int y) override;
    virtual void keyPressed(int key) override;
    virtual void keyReleased(int key) override {}
    virtual void update(std::bitset<NUM_KEYBOARD_KEYS> keyStates, float deltaTime) override;

private:
    glm::vec3 tumble(float deltaX, float deltaY);

    bool m_AltMousePressed = false;
    float m_MoveSpeed = 5.0f;
    float m_RotateSpeed = 0.001f;

    glm::vec2 m_PrevPos;

    glm::vec3 m_Target = glm::vec3(0.0f, 0.0f, 0.5f);
};

}; // namespace vrender
