#pragma once

#include "app/key_codes.h"
#include "camera.h"
#include <bitset>

namespace vrender
{

class ArcballCameraController : public CameraController
{
public:
    ArcballCameraController(Camera* camera);

protected:
    virtual void mouseMoved(int x, int y) override;
    virtual void mousePress(int button, int mods, int x, int y) override;
    virtual void mouseRelease(int button, int x, int y) override {}
    virtual void keyPressed(int key) override;
    virtual void keyReleased(int key) override {}
    virtual void update(std::bitset<NUM_KEYBOARD_KEYS> keyStates) override;

private:
    bool m_AltPressed = false;
};

}; // namespace vrender
