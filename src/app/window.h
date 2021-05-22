#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <string>

class Window
{
public:
    Window(unsigned int width, unsigned int height, const std::string& title);
    Window(const std::string& title) : Window(DEFAULT_WIDTH, DEFAULT_HEIGHT, title) {}
    ~Window();

    Window(const Window& window) = delete;
    Window& operator=(const Window& window) = delete;

    VkResult createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const;

    inline bool shouldClose() const { return glfwWindowShouldClose(m_WindowInstance); }
    inline bool framebufferResized() const { return m_FramebufferResized; }

    inline unsigned int width() const { return m_Width; }
    inline unsigned int height() const { return m_Height; }
    inline VkExtent2D extent() const { return {static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height)}; }

    void setFrameBufferResized(bool resized) { m_FramebufferResized = resized; }

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    void init();

    GLFWwindow* m_WindowInstance;
    unsigned int m_Width, m_Height;
    std::string m_Title;

    bool m_FramebufferResized = false;

    static constexpr unsigned int DEFAULT_WIDTH = 800;
    static constexpr unsigned int DEFAULT_HEIGHT = 600;

};
