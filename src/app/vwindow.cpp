#include "vwindow.h"

VWindow::VWindow(unsigned int width, unsigned int height, const std::string& title)
    :
    m_Width(width),
    m_Height(height),
    m_Title(title)
{
    init();
}

VWindow::~VWindow()
{
    glfwDestroyWindow(m_WindowInstance);
    glfwTerminate(); // TODO: Move this to app termination or something similar, shouldn't init/terminate glfw on every new window?
}

void VWindow::init()
{
    // TODO: Move this to app init or something similar to avoid initializing multiple times
    if (!glfwInit()) {
        // TODO: Replace with proper error/logging system
        perror("Could not init GLFW library.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_WindowInstance = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_WindowInstance, this);
    glfwSetFramebufferSizeCallback(m_WindowInstance, VWindow::framebufferSizeCallback);

    if (!m_WindowInstance) {
        // TODO: Replace with proper error/logging system
        perror("Could not create window.");
    }
}

VkResult VWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const
{
    return glfwCreateWindowSurface(instance, m_WindowInstance, nullptr, surface);
}

void VWindow::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    VWindow* pWindow = reinterpret_cast<VWindow*>(glfwGetWindowUserPointer(window));
    pWindow->m_FramebufferResized = true;
    pWindow->m_Width = width;
    pWindow->m_Height = height;
}
