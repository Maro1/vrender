#pragma once

#include "app/events/events.hpp"
#include "app/key_codes.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <bitset>

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
    inline void setRotation(const glm::quat& rotation) { m_Rotation = rotation; }

    inline const glm::vec3& position() const { return m_Position; }
    inline const glm::quat& rotation() const { return m_Rotation; }

    void rotate(const glm::quat& angle);
    void translate(const glm::vec3& translation);

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

class CameraController : public EventHandler
{
public:
    CameraController(Camera* camera) : m_Camera(camera) {}
    virtual ~CameraController() = default;

    virtual void handle(const Event& event) override;

protected:
    virtual void mouseMoved(int x, int y) = 0;
    virtual void mousePress(int button, int mods, int x, int y) = 0;
    virtual void mouseRelease(int button, int x, int y) = 0;
    virtual void keyPressed(int key) = 0;
    virtual void keyReleased(int key) = 0;
    virtual void update(std::bitset<NUM_KEYBOARD_KEYS> keyStates, float deltaTime) = 0;

    Camera* m_Camera;
};
} // namespace vrender
