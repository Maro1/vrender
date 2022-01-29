#include "window.hpp"

#include "core/graphics_context.hpp"
#include "events/key_events.hpp"
#include "events/mouse_events.hpp"

namespace vrender
{

Window::Window(unsigned int width, unsigned int height, const std::string& title)
    : m_Width(width), m_Height(height), m_Title(title)
{
    init();
}

Window::~Window()
{
    glfwDestroyWindow(m_WindowInstance);
    glfwTerminate(); // TODO: Move this to app termination or something similar, shouldn't init/terminate glfw on every
                     // new window?
}

void Window::init()
{
    // TODO: Move this to app init or something similar to avoid initializing multiple times
    if (!glfwInit())
    {
        // TODO: Replace with proper error/logging system
        perror("Could not init GLFW library.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_WindowInstance = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_WindowInstance, this);
    glfwSetInputMode(m_WindowInstance, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    setCallbacks();

    if (!m_WindowInstance)
    {
        // TODO: Replace with proper error/logging system
        perror("Could not create window.");
    }
}

VkResult Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const
{
    return glfwCreateWindowSurface(instance, m_WindowInstance, nullptr, surface);
}

void Window::update()
{
    double deltaTime = glfwGetTime() - m_Time;
    m_Time = glfwGetTime();

    InputStateEvent e(m_KeyStates, deltaTime);
    handleEvent(EventType::InputState, e);
    // TODO: NewFrameEvent e(deltaTime, keyStates);
}

void Window::setCallbacks()
{
    glfwSetFramebufferSizeCallback(m_WindowInstance, Window::framebufferSizeCallback);
    glfwSetKeyCallback(m_WindowInstance, Window::keyCallback);
    glfwSetMouseButtonCallback(m_WindowInstance, Window::mouseButtonCallback);
    glfwSetCursorPosCallback(m_WindowInstance, Window::mouseMoveCallback);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Window* pWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    pWindow->m_FramebufferResized = true;
    pWindow->m_Width = width;
    pWindow->m_Height = height;

    GraphicsContext::get().world()->camera()->setAspectRatio((float)width / height);

    WindowResizeEvent e(width, height);
    pWindow->handleEvent(EventType::WindowResize, e);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Window* pWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    switch (action)
    {
    case GLFW_PRESS: {
        pWindow->m_KeyStates[key] = 1;
        KeyPressedEvent e(scancode, 0);
        pWindow->handleEvent(EventType::KeyPress, e);
        break;
    }
    case GLFW_RELEASE: {
        pWindow->m_KeyStates[key] = 0;
        KeyReleasedEvent e(scancode);
        pWindow->handleEvent(EventType::KeyRelease, e);
        break;
    }
    case GLFW_REPEAT: {
        KeyPressedEvent e(scancode, 1);
        pWindow->handleEvent(EventType::KeyPress, e);
        break;
    }
    }
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Window* pWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    switch (action)
    {
    case GLFW_PRESS: {
        MousePressedEvent e(button, x, y, mods);
        pWindow->handleEvent(EventType::MouseButtonPress, e);
        break;
    }
    case GLFW_RELEASE: {
        MouseReleasedEvent e(button, x, y);
        pWindow->handleEvent(EventType::MouseButtonRelease, e);
    }
    }
}

void Window::mouseMoveCallback(GLFWwindow* window, double x, double y)
{
    Window* pWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    MouseMovedEvent e(x, y);
    pWindow->handleEvent(EventType::MouseMove, e);
}

void Window::handleEvent(EventType type, const Event& event)
{
    for (EventHandler* handler : m_EventTypeHandlers.at(type))
    {
        handler->handle(event);
    }
}

void Window::registerHandler(EventHandler* handler, EventType eventType)
{
    m_EventTypeHandlers.at(eventType).push_back(handler);
}

void Window::registerHandler(EventHandler* handler, const std::vector<EventType>& eventTypes)
{
    m_EventHandlers.push_back(handler);
    for (EventType t : eventTypes)
    {
        m_EventTypeHandlers.at(t).push_back(handler);
    }
}

}; // namespace vrender
