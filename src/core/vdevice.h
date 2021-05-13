#pragma once

#include <vector>
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "app/vwindow.h"

struct VAppInfo;

struct VQueueFamilyIndices 
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    inline bool isComplete() { return graphicsFamily.has_value() && graphicsFamily.has_value(); }
};

struct VSwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VDevice 
{
public:
    VDevice(const VAppInfo& appInfo, VWindow& window);
    ~VDevice();

    VDevice(const VDevice& device) = delete;
    VDevice& operator=(const VDevice& device) = delete;

    inline const VSwapChainSupportDetails swapChainSupportDetails() { return getSwapChainSupport(m_PhysicalDevice); }
    inline const VkSurfaceKHR& surface() const { return m_Surface; }
    inline const VQueueFamilyIndices queueFamilyIndices() { return getQueueFamilies(m_PhysicalDevice); }
    inline const VkDevice& device() const { return m_Device; }
    inline const VkCommandPool& commandPool() const { return m_CommandPool; }
    inline const VkQueue& graphicsQueue() const { return m_GraphicsQueue; }
    
private:

    int createVulkanInstance(const VAppInfo& appInfo);
    int createLogicalDevice();

    // Debug messenger setup
    void setupDebugMessenger();
    void populateDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    // Extension / validation layer setup
    std::vector<const char*> getExtensions();
    void checkValidationLayers() const;
    
    // Physical device setup
    // TODO: Have these return status code / message or something to log what is wrong
    bool getPhysicalDevice(); // Returns false if no device available
    // TODO: Have user select device through GUI or something
    int checkPhysicalDevice(const VkPhysicalDevice& device); // Checks if physical device is suitable, returns "score"

    bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);

    VQueueFamilyIndices getQueueFamilies(const VkPhysicalDevice& device);
    VSwapChainSupportDetails getSwapChainSupport(const VkPhysicalDevice& device);

    VkResult createSurface();
    VkResult createCommandPool();

    VkInstance m_VulkanInstance;
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_Device;
    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;
    VkCommandPool m_CommandPool;
    VkSurfaceKHR m_Surface; // TODO: Move out of this class

    VWindow& m_WindowRef;

    float m_QueuePriority = 1.0f;

    VkDebugUtilsMessengerEXT m_DebugMessenger;

    const std::vector<const char*> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

};
