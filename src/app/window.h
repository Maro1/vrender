#pragma once

#include "app/key_codes.h"
#include "events/events.h"
#include "utils/noncopyable.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <bitset>
#include <map>
#include <string>
#include <vector>

namespace vrender
{
class Window : private NonCopyable
{
public:
    Window(unsigned int width, unsigned int height, const std::string& title);
    Window(const std::string& title) : Window(DEFAULT_WIDTH, DEFAULT_HEIGHT, title) {}
    ~Window();

    VkResult createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const;

    inline bool shouldClose() const { return glfwWindowShouldClose(m_WindowInstance); }
    inline bool framebufferResized() const { return m_FramebufferResized; }

    inline unsigned int width() const { return m_Width; }
    inline unsigned int height() const { return m_Height; }
    inline VkExtent2D extent() const { return {static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height)}; }

    void setFrameBufferResized(bool resized) { m_FramebufferResized = resized; }

    // Called every frame for poll events
    void update();

    void registerHandler(EventHandler* handler, EventType eventType);
    void registerHandler(EventHandler* handler, const std::vector<EventType>& eventType);
    void handleEvent(EventType type, const Event& event);

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mouseMoveCallback(GLFWwindow* window, double x, double y);

private:
    void init();
    void setCallbacks();

    GLFWwindow* m_WindowInstance;
    unsigned int m_Width, m_Height;
    std::string m_Title;

    bool m_FramebufferResized = false;

    std::map<EventType, std::vector<EventHandler*>> m_EventTypeHandlers = {{EventType::KeyPress, {}},
                                                                           {EventType::KeyRelease, {}},
                                                                           {EventType::KeyType, {}},
                                                                           {EventType::WindowClose, {}},
                                                                           {EventType::WindowResize, {}},
                                                                           {EventType::WindowMove, {}},
                                                                           {EventType::KeyPress, {}},
                                                                           {EventType::MouseButtonPress, {}},
                                                                           {EventType::MouseButtonRelease, {}},
                                                                           {EventType::MouseMove, {}},
                                                                           {EventType::MouseScroll, {}},
                                                                           {EventType::InputState, {}}

    };

    std::vector<EventHandler*> m_EventHandlers;

    double m_Time;
    std::bitset<NUM_KEYBOARD_KEYS> m_KeyStates;

    static constexpr unsigned int DEFAULT_WIDTH = 800;
    static constexpr unsigned int DEFAULT_HEIGHT = 600;
};
}; // namespace vrender
