#include "window.h"

#include "key_events.h"

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

void Window::setCallbacks()
{
    glfwSetFramebufferSizeCallback(m_WindowInstance, Window::framebufferSizeCallback);
    glfwSetKeyCallback(m_WindowInstance, Window::keyCallback);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Window* pWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    pWindow->m_FramebufferResized = true;
    pWindow->m_Width = width;
    pWindow->m_Height = height;

    WindowResizeEvent e(width, height);
    for (EventHandler* handler : pWindow->m_EventHandlers.at(EventType::WindowResize))
    {
        handler->handle(e);
    }
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Window* pWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    switch (action)
    {
    case GLFW_PRESS: {
        KeyPressedEvent e(scancode, 0);
        for (EventHandler* handler : pWindow->m_EventHandlers.at(EventType::KeyPress))
        {
            handler->handle(e);
        }
        break;
    }
    case GLFW_RELEASE: {
        KeyReleasedEvent e(scancode);
        for (EventHandler* handler : pWindow->m_EventHandlers.at(EventType::KeyRelease))
        {
            handler->handle(e);
        }
        break;
    }
    case GLFW_REPEAT: {
        KeyPressedEvent e(scancode, 1);
        for (EventHandler* handler : pWindow->m_EventHandlers.at(EventType::KeyPress))
        {
            handler->handle(e);
        }
        break;
    }
    }
}

void Window::registerHandler(EventHandler* handler, EventType eventType)
{
    m_EventHandlers.at(eventType).push_back(handler);
}

}; // namespace vrender
