#include "KRAKEN/core/graphics/vulkan/Context.h"
#include "KRAKEN/core/graphics/vulkan/Globals.h"
#include "KRAKEN/core/graphics/vulkan/Util.h"
#include "KRAKEN/Defines.h"
#include "KRAKEN/Types.h"
#include "KRAKEN/core/Globals.h"
#include "KRAKEN/core/Application.h"
#include <sstream>

namespace kraken::vulkan
{
    void createInstance(Context& context)
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
        platformExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(platformExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = platformExtensions.data();

#if KRAKEN_USE_ASSERTS
        std::vector<const char*> requiredValidationLayers{
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
                if (strcmp(requiredValidationLayers[i], availableLayers[j].layerName) == 0)
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
        VK_CHECK(vkCreateInstance(&instanceCreateInfo, VK_CPU_ALLOCATOR, &context.instance));
        KRAKEN_ASSERT_VALUE(context.instance);
    }

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
            debugBreak();
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

    void createDebugMessenger(Context& context)
    {
        vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT"));
        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT"));

        VkDebugUtilsMessengerCreateInfoEXT createInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;

        VK_CHECK(vkCreateDebugUtilsMessengerEXT(context.instance, &createInfo, VK_CPU_ALLOCATOR, &context.debugMessenger));
        KRAKEN_ASSERT_VALUE(context.debugMessenger);
    }

    void createSurface(Context& context)
    {
        context.surface = globals::APPLICATION->getWindow()->getSurface(context.instance);
    }

    void selectPhysicalDevice(Context& context)
    {
        uint32_t deviceCount{ 0 };
        VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &deviceCount, nullptr));
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &deviceCount, physicalDevices.data()));
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
            // perhaps even automate the selection process to chose based on properties
            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                selectedDevice = physicalDevices[i];
            }
        }
        context.physicalDevice = selectedDevice;
        KRAKEN_ASSERT_VALUE(context.physicalDevice);
    }

    bool_t queueFamilySupportsGraphicsAndCompute(const VkQueueFamilyProperties& properties)
    {
        return (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (properties.queueFlags & VK_QUEUE_COMPUTE_BIT);
    }

    bool_t queueFamilySupportsPresentation(Context& context, uint32_t queueFamilyIndex)
    {
        VkBool32 presentationSupported{};
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(context.physicalDevice, queueFamilyIndex, context.surface, &presentationSupported));
        return presentationSupported;
    }

    void createDevice(Context& context)
    {
        selectPhysicalDevice(context);

        uint32_t queueFamilyCount{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties(context.physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(context.physicalDevice, &queueFamilyCount, queueFamilies.data());

        // std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
        float_t queuePriority{ 1.0f };

        // bool_t graphicsComputeQueueCreateInfoPushed{ false };
        // bool_t presentQueueCreateInfoPushed{ false };

        VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };

        for (uint32_t i{ 0 }; i < queueFamilyCount; i++)
        {
            if (queueFamilySupportsGraphicsAndCompute(queueFamilies[i]) && queueFamilySupportsPresentation(context, i))
            {
                queueCreateInfo.queueFamilyIndex = i;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;
                context.graphicsComputePresentQueueFamilyIndex = i;
                break;
            }


            //if (graphicsComputeQueueCreateInfoPushed && presentQueueCreateInfoPushed)
            //{
            //    break;
            //}
            //
            //VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            //queueCreateInfo.queueFamilyIndex = i;
            //queueCreateInfo.pQueuePriorities = &queuePriority;
            //bool_t queueFound{ false };
            //
            //if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            // && (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            // && !graphicsComputeQueueCreateInfoPushed)
            //{
            //    context.graphicsComputeQueueIndex = i;
            //    queueCreateInfo.queueCount++;
            //    queueFound = true;
            //    graphicsComputeQueueCreateInfoPushed = true;
            //}
            //VkBool32 presentationSupport{ 0 };
            //VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(context.physicalDevice, i, context.surface, &presentationSupport));
            //if (presentationSupport && !presentQueueCreateInfoPushed)
            //{
            //    context.presentQueueIndex = i;
            //    queueCreateInfo.queueCount++;
            //    queueFound = true;
            //    presentQueueCreateInfoPushed = true;
            //}
            //if (queueFound)
            //{
            //    queueCreateInfos.push_back(queueCreateInfo);
            //}
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        // VkPhysicalDeviceVulkan12Features deviceFeaturesVulkan12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };

        // TODO: this should be checked, especially when optional extensions like VK_KHR_raytracing are wanted
        std::vector<const char*> enabledDeviceExtensions{};
        enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();

        VK_CHECK(vkCreateDevice(context.physicalDevice, &deviceCreateInfo, VK_CPU_ALLOCATOR, &context.device));
        KRAKEN_ASSERT_VALUE(context.device);

        // bool_t graphicsComputeQueueCreated{ false };
        // bool_t presentQueueCreated{ false };

        // for (uint32_t i{ 0 }; i < queueCreateInfos.size(); i++)
        // {
        //     for (uint32_t j{ 0 }; j < queueCreateInfos[i].queueCount; j++)
        //     {
        //         if (!graphicsComputeQueueCreated && queueCreateInfos[i].queueFamilyIndex == context.graphicsComputeQueueIndex)
        //         {
        //             vkGetDeviceQueue(context.device, context.graphicsComputeQueueIndex, j, &context.graphicsComputeQueue);
        //             graphicsComputeQueueCreated = true;
        //             KRAKEN_CORE_TRACE("Created Graphics Compute Queue with Queueindex {0} and Queuefamily {1}", j, context.graphicsComputeQueueIndex);
        //             continue;
        //         }
        //         if (!presentQueueCreated)
        //         {
        //             vkGetDeviceQueue(context.device, context.presentQueueIndex, j, &context.presentQueue);
        //             presentQueueCreated = true;
        //             KRAKEN_CORE_TRACE("Created Present Queue with Queueindex {0} and Queuefamily {1}", j, context.presentQueueIndex);
        //             continue;
        //         }
        //     }
        // }

        vkGetDeviceQueue(context.device, queueCreateInfo.queueFamilyIndex, 0, &context.graphicsComputePresentQueue);

        KRAKEN_ASSERT_VALUE(context.graphicsComputePresentQueue);
        // KRAKEN_ASSERT_VALUE(context.graphicsComputeQueue);
        // KRAKEN_ASSERT_VALUE(context.presentQueue);
    }

    void getSwapchainImages(Context& context)
    {
        uint32_t swapchainImageCount{};
        VK_CHECK(vkGetSwapchainImagesKHR(context.device, context.swapchain, &swapchainImageCount, nullptr));
        context.swapchainImages.resize(swapchainImageCount);
        VK_CHECK(vkGetSwapchainImagesKHR(context.device, context.swapchain, &swapchainImageCount, context.swapchainImages.data()));
    }

    void createSwapchainImageViews(Context& context)
    {
        context.swapchainImageViews.resize(context.swapchainImages.size());

        VkImageViewCreateInfo imageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = context.swapchainImageFormat;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        for (uint32_t i{ 0 }; i < context.swapchainImages.size(); i++)
        {
            imageViewCreateInfo.image = context.swapchainImages[i];
            VK_CHECK(vkCreateImageView(context.device, &imageViewCreateInfo, VK_CPU_ALLOCATOR, &context.swapchainImageViews[i]));
        }
    }

    void createSwapchain(Context& context)
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.physicalDevice, context.surface, &surfaceCapabilities);

        VkSwapchainCreateInfoKHR swapchainCreateInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainCreateInfo.surface = context.surface;

        if (surfaceCapabilities.minImageCount <= 2 && surfaceCapabilities.maxImageCount >= 2)
        {
            swapchainCreateInfo.minImageCount = 2;
        }
        else
        {
            KRAKEN_CORE_CRITICAL("Double buffering not available.");
        }

        // TODO: add support for HDR and other displays here
        swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapchainCreateInfo.imageExtent = globals::APPLICATION->getWindow()->getFramebufferSize();
        context.swapchainImageExtent = swapchainCreateInfo.imageExtent;

        // TODO: we might want to write directly to the image later on
        if (surfaceCapabilities.supportedUsageFlags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        else
        {
            KRAKEN_CORE_CRITICAL("Swapchain does not support being written to.");
        }

        // if (context.presentQueueIndex != context.graphicsComputeQueueIndex)
        // {
        //     uint32_t queueFamilyIndices[2]{ context.presentQueueIndex, context.graphicsComputeQueueIndex };
        //     swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        //     swapchainCreateInfo.queueFamilyIndexCount = 2;
        //     swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        // }
        // else
        // {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        // }

        swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        uint32_t presentModeCount{ 0 };
        vkGetPhysicalDeviceSurfacePresentModesKHR(context.physicalDevice, context.surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(context.physicalDevice, context.surface, &presentModeCount, presentModes.data());

        swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.imageArrayLayers = 1;

        uint32_t surfaceFormatCount{ 0 };
        vkGetPhysicalDeviceSurfaceFormatsKHR(context.physicalDevice, context.surface, &surfaceFormatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> availableFormats(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(context.physicalDevice, context.surface, &surfaceFormatCount, availableFormats.data());

        bool bgraFound{ false };
        bool rgbaFound{ false };
        for (uint32_t i{ 0 }; i < surfaceFormatCount; i++)
        {
            if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                bgraFound = true;
            }
            else if (availableFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM)
            {
                rgbaFound = true;
            }
        }

        if (bgraFound)
        {
            swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        }
        else if (rgbaFound)
        {
            swapchainCreateInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
        }
        else
        {
            KRAKEN_CORE_CRITICAL("No viable surface format found");
        }

        context.swapchainImageFormat = swapchainCreateInfo.imageFormat;

        VK_CHECK(vkCreateSwapchainKHR(context.device, &swapchainCreateInfo, VK_CPU_ALLOCATOR, &context.swapchain));
        KRAKEN_ASSERT_VALUE(context.swapchain);

        getSwapchainImages(context);
        createSwapchainImageViews(context);
    }

    void createCommandPool(Context& context)
    {
        VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        createInfo.queueFamilyIndex = context.graphicsComputePresentQueueFamilyIndex;
        VK_CHECK(vkCreateCommandPool(context.device, &createInfo, VK_CPU_ALLOCATOR, &context.commandPool));
    }

    void printGpuMemoryInfo(Context& context)
    {
        VkPhysicalDeviceProperties deviceProperties{};
        vkGetPhysicalDeviceProperties(context.physicalDevice, &deviceProperties);
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(context.physicalDevice, &deviceMemoryProperties);

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

    void initContext(Context& context)
    {
        createInstance(context);
#if KRAKEN_USE_ASSERTS
        createDebugMessenger(context);
#endif
        createSurface(context);
        createDevice(context);
        createSwapchain(context);
        createCommandPool(context);

        printGpuMemoryInfo(context);
    }
    
    void freeContext(Context& context)
    {
        VK_CHECK(vkDeviceWaitIdle(context.device));

        vkDestroyCommandPool(context.device, context.commandPool, VK_CPU_ALLOCATOR);

        for (uint32_t i{ 0 }; i < context.swapchainImageViews.size(); i++)
        {
            vkDestroyImageView(context.device, context.swapchainImageViews[i], VK_CPU_ALLOCATOR);
        }

        vkDestroySwapchainKHR(context.device, context.swapchain, VK_CPU_ALLOCATOR);
        vkDestroyDevice(context.device, VK_CPU_ALLOCATOR);
        vkDestroySurfaceKHR(context.instance, context.surface, VK_CPU_ALLOCATOR);
#if KRAKEN_USE_ASSERTS
        vkDestroyDebugUtilsMessengerEXT(context.instance, context.debugMessenger, VK_CPU_ALLOCATOR);
#endif
        vkDestroyInstance(context.instance, VK_CPU_ALLOCATOR);
    }
}
