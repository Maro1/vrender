#include "device.hpp"

#include "app/vrender.hpp"
#include "utils/log.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <set>

namespace vrender
{

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData)
{
    V_LOG_DEBUG(pCallbackData->pMessage);

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

Device::Device(const AppInfo& appInfo, Window* window) : m_Window(window)
{
    createVulkanInstance(appInfo);
#ifndef NDEBUG
    setupDebugMessenger();
#endif
    VkResult createSurfaceResult = createSurface(); // TODO: Check result
    if (!getPhysicalDevice())
    {
        V_LOG_ERROR("Could not find any device!");
        // TODO: Exit app
    }
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);

    createLogicalDevice();
    createCommandPool();
    getMemoryProperties();
}

Device::~Device()
{
    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    vkDestroyDevice(m_Device, nullptr);
#ifndef NDEBUG
    DestroyDebugUtilsMessengerEXT(m_VulkanInstance, m_DebugMessenger, nullptr);
#endif
    vkDestroySurfaceKHR(m_VulkanInstance, m_Surface, nullptr);
    vkDestroyInstance(m_VulkanInstance, nullptr);
}

int Device::createVulkanInstance(const AppInfo& appInfo)
{
    /* Application Info */
    VkApplicationInfo vulkanAppInfo;
    vulkanAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vulkanAppInfo.apiVersion = VK_MAKE_VERSION(appInfo.apiMajor, appInfo.apiMinor, 0);
    ;
    vulkanAppInfo.applicationVersion = VK_MAKE_VERSION(appInfo.apiMajor, appInfo.apiMinor, 0);
    vulkanAppInfo.engineVersion = VK_MAKE_VERSION(appInfo.apiMajor, appInfo.apiMinor, 0);
    vulkanAppInfo.pApplicationName = appInfo.title.c_str();
    vulkanAppInfo.pEngineName = appInfo.title.c_str();
    vulkanAppInfo.pNext = NULL;

    /* Instance Info */
    VkInstanceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &vulkanAppInfo;
    createInfo.pNext = NULL;
    createInfo.flags = 0;

    std::vector<const char*> extensions = getExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

#ifndef NDEBUG
    checkValidationLayers();
#endif

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
#ifndef NDEBUG
    createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
    createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

    populateDebugMessengerInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#else
    createInfo.enabledLayerCount = 0;
#endif

    //! Propably better to handle errors here and return custom error type
    return vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance);
}

int Device::createLogicalDevice()
{
    QueueFamilyIndices queueIndicies = getQueueFamilies(m_PhysicalDevice);

    if (!queueIndicies.isComplete())
    {
        V_LOG_ERROR("Could not find suitable queue family.");
        return -1;
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {queueIndicies.graphicsFamily.value(),
                                              queueIndicies.presentFamily.value()};

    for (const uint32_t& queueFamilyIndex : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.flags = 0;
        queueCreateInfo.pNext = NULL;
        queueCreateInfo.pQueuePriorities = &m_QueuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.pNext = NULL;
    createInfo.flags = 0;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

#ifndef NDEBUG
    createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
    createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    //! Propably better to handle errors here and return custom error type
    VkResult createDeviceResult = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);

    vkGetDeviceQueue(m_Device, queueIndicies.graphicsFamily.value(), 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, queueIndicies.presentFamily.value(), 0, &m_PresentQueue);

    return createDeviceResult;
}

std::vector<const char*> Device::getExtensions()
{
    uint32_t glfwExtensionCount;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

    for (unsigned int i = 0; i < glfwExtensionCount; i++)
    {
        const char* extension = glfwExtensions[i];

        // If glfw extension not in extensions vector
        if (std::find_if(availableExtensions.begin(), availableExtensions.end(), [&extension](auto x) {
                return !std::string(x.extensionName).compare(extension);
            }) == availableExtensions.end())
        {
            V_LOG_WARNING("Could not find extension: {}", extension);
        }
    }

    return extensions;
}

void Device::checkValidationLayers() const
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    V_LOG_DEBUG("Available validation layers:");
    for (const auto& layer : availableLayers)
    {
        V_LOG_DEBUG("\t{}", layer.layerName);
    }

    for (const char* layer : m_ValidationLayers)
    {
        bool found = false;

        // If layer not in available layers vector
        if (std::find_if(availableLayers.begin(), availableLayers.end(), [&layer](auto x) {
                return !std::string(x.layerName).compare(layer);
            }) == availableLayers.end())
        {
            V_LOG_WARNING("Could not find validation layer: {}", layer); // TODO: Use logging system
        }
    }
}

void Device::populateDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
    createInfo.flags = 0;
    createInfo.pNext = NULL;
}

void Device::setupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(m_VulkanInstance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
    {
        V_LOG_WARNING("Could not create debug messenger."); // TODO: Print VkResult nicely
    }
}

bool Device::checkDeviceExtensionSupport(const VkPhysicalDevice& device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());
    for (const auto& ex : requiredExtensions)
    {
        auto found = std::find_if(availableExtensions.begin(), availableExtensions.end(),
                                  [&ex](auto x) { return x.extensionName == ex; });
        if (found == availableExtensions.end())
        {
            V_LOG_ERROR("Could not find device extension {}", ex);
            return false;
        }
        else
        {
            V_LOG_DEBUG("Found device extension {}", found->extensionName);
        }
    }
    return true;
}

int Device::checkPhysicalDevice(const VkPhysicalDevice& device)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures); // TODO: Check these if needed

    int score = 0;

    if (!getQueueFamilies(device).isComplete())
        return 0; // Must have complete queue families
    if (!checkDeviceExtensionSupport(device))
        return 0; // Must support required extensions

    SwapChainSupportDetails swapChainSupport = getSwapChainSupport(device);
    if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
        return 0; // Must have at least one format and present mode

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }
    score += deviceProperties.limits.maxImageDimension2D;
    return score;
}

bool Device::getPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, nullptr);

    if (!deviceCount)
        return false; // No device supporting Vulkan

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, devices.data());

    std::map<int, VkPhysicalDevice> deviceScores;
    for (const VkPhysicalDevice& device : devices)
    {
        int score = checkPhysicalDevice(device);
        deviceScores.insert(std::make_pair(score, device));
    }

    if (deviceScores.rbegin()->first == 0)
        return false; // No physical device meets requirements of application

    m_PhysicalDevice = deviceScores.rbegin()->second;

    return true;
}

QueueFamilyIndices Device::getQueueFamilies(const VkPhysicalDevice& device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    unsigned int i = 0;
    VkBool32 presentSupport = false;
    for (const VkQueueFamilyProperties& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
        if (presentSupport)
        {
            indices.presentFamily = i;
            presentSupport = false;
        }

        if (indices.isComplete())
            break;

        i++;
    }

    return indices;
}

SwapChainSupportDetails Device::getSwapChainSupport(const VkPhysicalDevice& device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkResult Device::createSurface()
{
    return m_Window->createWindowSurface(m_VulkanInstance, &m_Surface);
}

VkResult Device::createCommandPool()
{
    QueueFamilyIndices familyIndices = queueFamilyIndices();

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = familyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    return vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool);
}

void Device::getMemoryProperties()
{
    VkPhysicalDeviceMemoryProperties properties;
}

}; // namespace vrender
