#include "KRAKEN/core/renderer/Renderer.h"
#include "KRAKEN/core/Globals.h"
#include <vector>
#include "KRAKEN/core/Application.h"
#include <map>

namespace kraken
{
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
        )
    {
        KRAKEN_UNUSED_VARIABLE(pUserData);
        KRAKEN_UNUSED_VARIABLE(type);
        if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            ::kraken::Logger::getValidationErrorLogger()->error("{0}", pCallbackData->pMessage);
        }
        else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            ::kraken::Logger::getValidationErrorLogger()->warn("{0}", pCallbackData->pMessage);
        }
        else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            ::kraken::Logger::getValidationErrorLogger()->trace("{0}", pCallbackData->pMessage);
        }
        return VK_FALSE;
    }

    void Renderer::createInstance()
    {
        VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.pApplicationName = constants::ENGINE_NAME;
        appInfo.applicationVersion = VK_MAKE_VERSION(constants::VERSION_MAJOR, constants::VERSION_MINOR, constants::VERSION_PATCH);
        appInfo.pEngineName = constants::ENGINE_NAME;
        appInfo.engineVersion = VK_MAKE_VERSION(constants::VERSION_MAJOR, constants::VERSION_MINOR, constants::VERSION_PATCH);

        VkInstanceCreateInfo instanceCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instanceCreateInfo.pApplicationInfo = &appInfo;

        std::vector<const char*> platformExtensions{};
        const char** platformExtensionsCstr{};
        uint32_t platformExtensionCount{ 0 };
        globals::APPLICATION->getWindow()->getRequiredPlatformExtensions(&platformExtensionCount, &platformExtensionsCstr);
        for (uint32_t i{ 0 }; i < platformExtensionCount; i++)
        {
            platformExtensions.emplace_back(platformExtensionsCstr[i]);
        }
        platformExtensions.emplace_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(platformExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = platformExtensions.data();

#if KRAKEN_USE_ASSERTS
        std::vector<const char*> requiredValidationLayers {
            "VK_LAYER_KHRONOS_validation"
        };

        uint32_t availableInstanceLayerCount{ 0 };
        VK_CHECK(vkEnumerateInstanceLayerProperties(&availableInstanceLayerCount, nullptr));
        std::vector<VkLayerProperties> availableLayers(availableInstanceLayerCount);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&availableInstanceLayerCount, availableLayers.data()));

        for (uint32_t i{ 0 }; i < requiredValidationLayers.size(); i++)
        {
            bool_t found{ 0 };
            for (uint32_t j{ 0 }; j < availableInstanceLayerCount; j++)
            {
                if ( strcmp(requiredValidationLayers[i], availableLayers[j].layerName) == 0 )
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                KRAKEN_CORE_CRITICAL("Validation Layer unavailable: {0}", requiredValidationLayers[i]);
            }
        }

        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
#else
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.ppEnabledLayerNames = nullptr;
#endif
        VK_CHECK(vkCreateInstance(&instanceCreateInfo, vulkan::VK_CPU_ALLOCATOR, &vulkan::INSTANCE));
    }

    void Renderer::setupDebugMessenger()
    {
        vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(vulkan::INSTANCE, "vkCreateDebugUtilsMessengerEXT"));
        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(vulkan::INSTANCE, "vkDestroyDebugUtilsMessengerEXT"));

        VkDebugUtilsMessengerCreateInfoEXT createInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;

        VK_CHECK(vkCreateDebugUtilsMessengerEXT(vulkan::INSTANCE, &createInfo, vulkan::VK_CPU_ALLOCATOR, &this->debugMessenger));
    }

    void Renderer::selectPhysicalDevice()
    {
        uint32_t deviceCount{ 0 };
        vkEnumeratePhysicalDevices(vulkan::INSTANCE, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(vulkan::INSTANCE, &deviceCount, physicalDevices.data());
        KRAKEN_ASSERT_VALUE(physicalDevices.size() > 0);

        VkPhysicalDevice fallbackDevice{ physicalDevices[0] };
        VkPhysicalDevice selectedDevice{ fallbackDevice };
        for (uint32_t i{ 0 }; i < physicalDevices.size(); i++)
        {
            VkPhysicalDeviceProperties deviceProperties{};
            vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
            // VkPhysicalDeviceFeatures   deviceFeatures{};
            // vkGetPhysicalDeviceFeatures(physicalDevices[i], &deviceFeatures);

            // TODO: we pick the first discrete GPU here. This should be able to be chosen at will.
            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                selectedDevice = physicalDevices[i];
            }
        }
        vulkan::PHYSICAL_DEVICE = selectedDevice;
    }

    void Renderer::logPhysicalDeviceInfo()
    {
        VkPhysicalDeviceProperties deviceProperties{};
        vkGetPhysicalDeviceProperties(vulkan::PHYSICAL_DEVICE, &deviceProperties);
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(vulkan::PHYSICAL_DEVICE, &deviceMemoryProperties);

        KRAKEN_CORE_INFO("Selected Device: {0}", deviceProperties.deviceName);
        KRAKEN_CORE_INFO("Driver Version: {0}", deviceProperties.driverVersion);
        KRAKEN_CORE_INFO("Memory:");
        for (uint32_t i{ 0 }; i < deviceMemoryProperties.memoryHeapCount; i++)
        {
            KRAKEN_CORE_INFO("\tHeap Size: {0:.6f} GB", deviceMemoryProperties.memoryHeaps[i].size * 1.0e-9);
            std::stringstream heapTypeStringStream{};
            heapTypeStringStream << "\t\tHeapType: ";
            for (uint32_t j{ 0 }; j < deviceMemoryProperties.memoryTypeCount; j++)
            {
                if (deviceMemoryProperties.memoryTypes[j].heapIndex == i)
                {
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                    {
                        heapTypeStringStream << "DEVICE_LOCAL\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD)
                    {
                        heapTypeStringStream << "DEVICE_DEVICE_COHERENT_AMD\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD)
                    {
                        heapTypeStringStream << "DEVICE_UNCACHED_AMD\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
                    {
                        heapTypeStringStream << "HOST_VISIBLE\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
                    {
                        heapTypeStringStream << "HOST_COHERENT\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
                    {
                        heapTypeStringStream << "HOST_CACHED\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
                    {
                        heapTypeStringStream << "LAZILY_ALLOCATED\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT)
                    {
                        heapTypeStringStream << "PROTECTED\t";
                    }
                }
            }
            KRAKEN_CORE_INFO(heapTypeStringStream.str().c_str());
        }
    }

    void Renderer::createSurface()
    {
        this->surface = globals::APPLICATION->getWindow()->getSurface();
    }

    void Renderer::findQueueFamilies()
    {
        uint32_t queueFamilyCount{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties(vulkan::PHYSICAL_DEVICE, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(vulkan::PHYSICAL_DEVICE, &queueFamilyCount, queueFamilies.data());
        for (uint32_t i{ 0 }; i < queueFamilyCount; i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                vulkan::GRAPHICS_QUEUE_INDEX = i;
            }
            if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                vulkan::COMPUTE_QUEUE_INDEX = i;
            }
            VkBool32 presentationSupport{ 0 };
            vkGetPhysicalDeviceSurfaceSupportKHR(vulkan::PHYSICAL_DEVICE, i, surface, &presentationSupport);
            if (presentationSupport)
            {
                vulkan::PRESENT_QUEUE_INDEX = i;
            }
        }
    }

    void Renderer::createDevice()
    {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        graphicsQueueCreateInfo.queueFamilyIndex = vulkan::GRAPHICS_QUEUE_INDEX;

        VkDeviceQueueCreateInfo computeQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        computeQueueCreateInfo.queueFamilyIndex = vulkan::COMPUTE_QUEUE_INDEX;

        VkDeviceQueueCreateInfo presentationQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        presentationQueueCreateInfo.queueFamilyIndex = vulkan::PRESENT_QUEUE_INDEX;

        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

        vkCreateDevice(vulkan::PHYSICAL_DEVICE, &deviceCreateInfo, vulkan::VK_CPU_ALLOCATOR, &vulkan::DEVICE);
    }

    void Renderer::init()
    {
        createInstance();
#if KRAKEN_USE_ASSERTS
        setupDebugMessenger();
#endif
        selectPhysicalDevice();
        logPhysicalDeviceInfo();
        createSurface();
        findQueueFamilies();
        createDevice();
    }

    void Renderer::free()
    {
        vkDestroyDevice(vulkan::DEVICE, vulkan::VK_CPU_ALLOCATOR);
        vkDestroySurfaceKHR(vulkan::INSTANCE, this->surface, vulkan::VK_CPU_ALLOCATOR);
#if KRAKEN_USE_ASSERTS
        vkDestroyDebugUtilsMessengerEXT(vulkan::INSTANCE, this->debugMessenger, vulkan::VK_CPU_ALLOCATOR);
#endif
        vkDestroyInstance(vulkan::INSTANCE, vulkan::VK_CPU_ALLOCATOR);
    }
}
