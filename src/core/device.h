#pragma once

#include <optional>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "app/window.h"
#include "utils/noncopyable.h"

namespace vrender
{

struct AppInfo;

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    inline bool isComplete() { return graphicsFamily.has_value() && graphicsFamily.has_value(); }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Device : private NonCopyable
{
public:
    Device(const AppInfo& appInfo, Window* window);
    ~Device();

    inline const SwapChainSupportDetails swapChainSupportDetails() { return getSwapChainSupport(m_PhysicalDevice); }
    inline const VkSurfaceKHR surface() const { return m_Surface; }
    inline const QueueFamilyIndices queueFamilyIndices() { return getQueueFamilies(m_PhysicalDevice); }
    inline const VkDevice device() const { return m_Device; }
    inline const VkPhysicalDevice physicalDevice() const { return m_PhysicalDevice; }
    inline const VkCommandPool commandPool() const { return m_CommandPool; }
    inline const VkQueue graphicsQueue() const { return m_GraphicsQueue; }
    inline VkDeviceSize memorySize() const { return m_MemorySize; };
    inline const VkPhysicalDeviceMemoryProperties memoryProperties() const { return m_MemoryProperties; }
    inline const VkPhysicalDeviceLimits limits() const { return m_Properties.limits; }

private:
    int createVulkanInstance(const AppInfo& appInfo);
    int createLogicalDevice();

    // Debug messenger setup
    void setupDebugMessenger();
    void populateDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    // Extension / validation layer setup
    std::vector<const char*> getExtensions();
    void checkValidationLayers() const;

    // Physical device setup
    // TODO: Have these return status code / message or something to log what is wrong
    bool getPhysicalDevice(); // Returns false if no device available
    // TODO: Have user select device through GUI or something
    int checkPhysicalDevice(const VkPhysicalDevice& device); // Checks if physical device is suitable, returns "score"

    bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);

    QueueFamilyIndices getQueueFamilies(const VkPhysicalDevice& device);
    SwapChainSupportDetails getSwapChainSupport(const VkPhysicalDevice& device);

    VkResult createSurface();
    VkResult createCommandPool();

    void getMemoryProperties();

    VkInstance m_VulkanInstance;
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_Device;
    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;
    VkCommandPool m_CommandPool;
    VkSurfaceKHR m_Surface; // TODO: Move out of this class
    VkDeviceSize m_MemorySize;

    VkPhysicalDeviceMemoryProperties m_MemoryProperties;
    VkPhysicalDeviceProperties m_Properties;

    Window* m_Window;

    float m_QueuePriority = 1.0f;

    VkDebugUtilsMessengerEXT m_DebugMessenger;

    const std::vector<const char*> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
}; // namespace vrender
