#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/graphics/vulkan/Context.h"
#include "Yggdrasil/core/graphics/vulkan/Globals.h"
#include "Yggdrasil/core/graphics/vulkan/Util.h"
#include "Yggdrasil/Defines.h"
#include "Yggdrasil/Types.h"
#include "Yggdrasil/core/Globals.h"
#include "Yggdrasil/core/Application.h"
#include "Yggdrasil/core/util/Log.h"
#include <sstream>
#include <set>

namespace yggdrasil::vulkan
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

#if YGGDRASIL_USE_ASSERTS
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
                YGGDRASIL_CORE_CRITICAL("Validation Layer unavailable: {0}", requiredValidationLayers[i]);
            }
        }

        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
#else
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.ppEnabledLayerNames = nullptr;
#endif
        VK_CHECK(vkCreateInstance(&instanceCreateInfo, VK_CPU_ALLOCATOR, &context.instance));
        YGGDRASIL_ASSERT_VALUE(context.instance);
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
        YGGDRASIL_UNUSED_VARIABLE(pUserData);
        YGGDRASIL_UNUSED_VARIABLE(type);
        if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            ::yggdrasil::Logger::getValidationErrorLogger()->error("{0}", pCallbackData->pMessage);
            debugBreak();
        }
        else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            ::yggdrasil::Logger::getValidationErrorLogger()->warn("{0}", pCallbackData->pMessage);
        }
        else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            ::yggdrasil::Logger::getValidationErrorLogger()->trace("{0}", pCallbackData->pMessage);
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
        YGGDRASIL_ASSERT_VALUE(context.debugMessenger);
    }

    void createSurface(Context& context)
    {
        context.screen.surface = globals::APPLICATION->getWindow()->getSurface(context.instance);
    }

    void selectPhysicalDevice(Context& context)
    {
        uint32_t deviceCount{ 0 };
        VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &deviceCount, nullptr));
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &deviceCount, physicalDevices.data()));
        YGGDRASIL_ASSERT_VALUE(physicalDevices.size() > 0);

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
        context.device.physical = selectedDevice;
        YGGDRASIL_ASSERT_VALUE(context.device.physical);
    }

    bool_t queueFamilySupportsGraphicsAndCompute(const VkQueueFamilyProperties& properties)
    {
        return (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (properties.queueFlags & VK_QUEUE_COMPUTE_BIT);
    }

    bool_t queueFamilySupportsPresentation(Context& context, uint32_t queueFamilyIndex)
    {
        VkBool32 presentationSupported{};
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(context.device.physical, queueFamilyIndex, context.screen.surface, &presentationSupported));
        return presentationSupported;
    }

    void createDevice(Context& context)
    {
        selectPhysicalDevice(context);

        uint32_t queueFamilyCount{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties(context.device.physical, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(context.device.physical, &queueFamilyCount, queueFamilies.data());

        bool_t transferQueueFamilyFound{ false };
        bool_t asyncComputeQueueFamilyFound{ false };
        bool_t rasterizerQueueFamilyFound{ false };
        bool_t presentQueueFamilyFound{ false };

        for (uint32_t i{ 0 }; i < queueFamilies.size(); i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
                queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT &&
                queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT &&
                !rasterizerQueueFamilyFound)
            {
                context.queues.rasterizerQueueFamilyIndex = i;
                rasterizerQueueFamilyFound = true;
                if (!presentQueueFamilyFound &&
                    queueFamilySupportsPresentation(context, i))
                {
                    presentQueueFamilyFound = true;
                    context.queues.presentQueueFamilyIndex = i;
                }
                else if (queueFamilySupportsPresentation(context, i))
                {
                    context.queues.presentQueueFamilyIndex = i;
                }

                YGGDRASIL_CORE_INFO("Found main rasterizer queue family with index {0}.", i);
            }
            else if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT &&
                     queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT &&
                     !asyncComputeQueueFamilyFound)
            {
                context.queues.asyncComputeQueueFamilyIndex = i;
                asyncComputeQueueFamilyFound = true;

                YGGDRASIL_CORE_INFO("Found async compute queue family with index {0}.", i);
            }
            else if(queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT &&
                   !transferQueueFamilyFound)
            {
                context.queues.transferQueueFamilyIndex = i;
                transferQueueFamilyFound = true;

                YGGDRASIL_CORE_INFO("Found transfer queue family with index {0}.", i);
            }
        }

        YGGDRASIL_ASSERT_VALUE_MSG(rasterizerQueueFamilyFound, "No main queue family found. Aborting.");

        std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos{};

        float_t queuePriorities[]{ 1.0f, 1.0f };
        if (asyncComputeQueueFamilyFound)
        {
            VkDeviceQueueCreateInfo asyncComputeQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            asyncComputeQueueCreateInfo.pQueuePriorities = queuePriorities;
            asyncComputeQueueCreateInfo.queueFamilyIndex = context.queues.asyncComputeQueueFamilyIndex;
            asyncComputeQueueCreateInfo.queueCount = 1;
            deviceQueueCreateInfos.push_back(asyncComputeQueueCreateInfo);
        }
        if (transferQueueFamilyFound)
        {
            VkDeviceQueueCreateInfo transferQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            transferQueueCreateInfo.pQueuePriorities = queuePriorities;
            transferQueueCreateInfo.queueFamilyIndex = context.queues.transferQueueFamilyIndex;
            transferQueueCreateInfo.queueCount = 1;
            deviceQueueCreateInfos.push_back(transferQueueCreateInfo);
        }
        VkDeviceQueueCreateInfo mainQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        mainQueueCreateInfo.pQueuePriorities = queuePriorities;
        mainQueueCreateInfo.queueFamilyIndex = context.queues.rasterizerQueueFamilyIndex;
        if (presentQueueFamilyFound &&
            context.queues.presentQueueFamilyIndex == context.queues.rasterizerQueueFamilyIndex)
        {
            mainQueueCreateInfo.queueCount = 2;
        }
        else
        {
            mainQueueCreateInfo.queueCount = 1;
        }
        deviceQueueCreateInfos.push_back(mainQueueCreateInfo);

        VkPhysicalDeviceFeatures deviceFeatures{};

        // VkPhysicalDeviceVulkan12Features deviceFeaturesVulkan12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };

        // TODO: this should be checked, especially when optional extensions like VK_KHR_raytracing are wanted
        std::vector<const char*> enabledDeviceExtensions{};
        enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();

        VK_CHECK(vkCreateDevice(context.device.physical, &deviceCreateInfo, VK_CPU_ALLOCATOR, &context.device.logical));
        YGGDRASIL_ASSERT_VALUE(context.device.logical);

        vkGetDeviceQueue(context.device.logical, context.queues.rasterizerQueueFamilyIndex, 0, &context.queues.rasterizerQueue);
        if (!asyncComputeQueueFamilyFound)
        {
            context.queues.asyncComputeQueue = context.queues.rasterizerQueue;
        }
        else
        {
             vkGetDeviceQueue(context.device.logical, context.queues.asyncComputeQueueFamilyIndex, 0, &context.queues.asyncComputeQueue);
        }
        if (!transferQueueFamilyFound)
        {
            context.queues.transferQueue = context.queues.rasterizerQueue;
        }
        else
        {
            vkGetDeviceQueue(context.device.logical, context.queues.transferQueueFamilyIndex, 0, &context.queues.transferQueue);
        }
        if (presentQueueFamilyFound)
        {
            context.queues.presentQueue = context.queues.rasterizerQueue;
        }
        else
        {
            YGGDRASIL_CORE_ERROR("Presentation not supported.");
        }

        YGGDRASIL_ASSERT_VALUE(context.queues.rasterizerQueue);
        YGGDRASIL_ASSERT_VALUE(context.queues.presentQueue);
    }

    void getSwapchainImages(Context& context)
    {
        uint32_t swapchainImageCount{};
        VK_CHECK(vkGetSwapchainImagesKHR(context.device.logical, context.screen.swapchain, &swapchainImageCount, nullptr));
        context.screen.swapchainImages.resize(swapchainImageCount);
        VK_CHECK(vkGetSwapchainImagesKHR(context.device.logical, context.screen.swapchain, &swapchainImageCount, context.screen.swapchainImages.data()));
    }

    void createSwapchainImageViews(Context& context)
    {
        context.screen.swapchainImageViews.resize(context.screen.swapchainImages.size());

        VkImageViewCreateInfo imageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = context.screen.swapchainImageFormat;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        for (uint32_t i{ 0 }; i < context.screen.swapchainImages.size(); i++)
        {
            imageViewCreateInfo.image = context.screen.swapchainImages[i];
            VK_CHECK(vkCreateImageView(context.device.logical, &imageViewCreateInfo, VK_CPU_ALLOCATOR, &context.screen.swapchainImageViews[i]));
        }
    }

    void createSwapchain(Context& context)
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.device.physical, context.screen.surface, &surfaceCapabilities);

        VkSwapchainCreateInfoKHR swapchainCreateInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainCreateInfo.surface = context.screen.surface;

        if (surfaceCapabilities.minImageCount <= 2 && surfaceCapabilities.maxImageCount >= 2)
        {
            swapchainCreateInfo.minImageCount = 2;
        }
        else
        {
            YGGDRASIL_CORE_CRITICAL("Double buffering not available.");
        }

        // TODO: add support for HDR and other displays here
        swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapchainCreateInfo.imageExtent = globals::APPLICATION->getWindow()->getFramebufferSize();
        context.screen.swapchainImageExtent = swapchainCreateInfo.imageExtent;

        // TODO: we might want to write directly to the image later on
        if (surfaceCapabilities.supportedUsageFlags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        else
        {
            YGGDRASIL_CORE_CRITICAL("Swapchain does not support being written to.");
        }

        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        uint32_t presentModeCount{ 0 };
        vkGetPhysicalDeviceSurfacePresentModesKHR(context.device.physical, context.screen.surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(context.device.physical, context.screen.surface, &presentModeCount, presentModes.data());

        swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.imageArrayLayers = 1;

        uint32_t surfaceFormatCount{ 0 };
        vkGetPhysicalDeviceSurfaceFormatsKHR(context.device.physical, context.screen.surface, &surfaceFormatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> availableFormats(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(context.device.physical, context.screen.surface, &surfaceFormatCount, availableFormats.data());

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
            YGGDRASIL_CORE_CRITICAL("No viable surface format found");
        }

        context.screen.swapchainImageFormat = swapchainCreateInfo.imageFormat;

        VK_CHECK(vkCreateSwapchainKHR(context.device.logical, &swapchainCreateInfo, VK_CPU_ALLOCATOR, &context.screen.swapchain));
        YGGDRASIL_ASSERT_VALUE(context.screen.swapchain);

        getSwapchainImages(context);
        createSwapchainImageViews(context);
    }

    void createCommandPools(Context& context)
    {
        VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        createInfo.queueFamilyIndex = context.queues.rasterizerQueueFamilyIndex;

        VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;

        context.commandPools.resize(context.screen.swapchainImages.size());
        context.commandBuffers.resize(context.screen.swapchainImages.size());
        for (uint32_t i{ 0 }; i < context.screen.swapchainImages.size(); i++)
        {
            VK_CHECK(vkCreateCommandPool(context.device.logical, &createInfo, VK_CPU_ALLOCATOR, &context.commandPools[i]));
            allocateInfo.commandPool = context.commandPools[i];
            VK_CHECK(vkAllocateCommandBuffers(context.device.logical, &allocateInfo, &context.commandBuffers[i]));
        }
    }

    void createSyncObjects(Context& context)
    {
        context.syncObjects.acquireFences.resize(context.screen.swapchainImages.size());
        context.syncObjects.acquireSemaphores.resize(context.screen.swapchainImages.size());
        context.syncObjects.releaseSemaphores.resize(context.screen.swapchainImages.size());
        for (uint32_t i{ 0 }; i < context.screen.swapchainImages.size(); i++)
        {
            context.syncObjects.acquireFences[i] = util::createFence(context.device.logical, VK_FENCE_CREATE_SIGNALED_BIT);
            context.syncObjects.acquireSemaphores[i] = util::createSemaphore(context.device.logical);
            context.syncObjects.releaseSemaphores[i] = util::createSemaphore(context.device.logical);
        }
    }

    void freeSyncObjects(Context& context)
    {
        for (uint32_t i{ 0 }; i < context.screen.swapchainImages.size(); i++)
        {
            vkDestroyFence(context.device.logical, context.syncObjects.acquireFences[i], VK_CPU_ALLOCATOR);
            vkDestroySemaphore(context.device.logical, context.syncObjects.acquireSemaphores[i], VK_CPU_ALLOCATOR);
            vkDestroySemaphore(context.device.logical, context.syncObjects.releaseSemaphores[i], VK_CPU_ALLOCATOR);
        }
    }

    void printGpuMemoryInfo(Context& context)
    {
        VkPhysicalDeviceProperties deviceProperties{};
        vkGetPhysicalDeviceProperties(context.device.physical, &deviceProperties);
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(context.device.physical, &deviceMemoryProperties);

        YGGDRASIL_CORE_INFO("Selected Device: {0}", deviceProperties.deviceName);
        YGGDRASIL_CORE_INFO("Driver Version: {0}", deviceProperties.driverVersion);
        YGGDRASIL_CORE_INFO("Memory:");
        for (uint32_t i{ 0 }; i < deviceMemoryProperties.memoryHeapCount; i++)
        {
            YGGDRASIL_CORE_INFO("\tHeap Size: {0:.6f} GB", deviceMemoryProperties.memoryHeaps[i].size * 1.0e-9);
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
            YGGDRASIL_CORE_INFO(heapTypeStringStream.str().c_str());
        }
    }

    void initContext(Context& context)
    {
        createInstance(context);
#if YGGDRASIL_USE_ASSERTS
        createDebugMessenger(context);
#endif
        createSurface(context);
        createDevice(context);
        createSwapchain(context);
        createCommandPools(context);
        createSyncObjects(context);

        printGpuMemoryInfo(context);
    }
    
    void freeContext(Context& context)
    {
        VK_CHECK(vkDeviceWaitIdle(context.device.logical));

        freeSyncObjects(context);
        for (uint32_t i{ 0 }; i < context.screen.swapchainImages.size(); i++)
        {
            vkDestroyCommandPool(context.device.logical, context.commandPools[i], VK_CPU_ALLOCATOR);
        }
        for (uint32_t i{ 0 }; i < context.screen.swapchainImageViews.size(); i++)
        {
            vkDestroyImageView(context.device.logical, context.screen.swapchainImageViews[i], VK_CPU_ALLOCATOR);
        }
        if (context.screen.swapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(context.device.logical, context.screen.swapchain, VK_CPU_ALLOCATOR);
        }
        if (context.device.logical != VK_NULL_HANDLE)
        {
            vkDestroyDevice(context.device.logical, VK_CPU_ALLOCATOR);
        }
        if (context.screen.surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(context.instance, context.screen.surface, VK_CPU_ALLOCATOR);
        }
#if YGGDRASIL_USE_ASSERTS
        if (context.debugMessenger != VK_NULL_HANDLE)
        {
            vkDestroyDebugUtilsMessengerEXT(context.instance, context.debugMessenger, VK_CPU_ALLOCATOR);
        }
#endif
        if (context.instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(context.instance, VK_CPU_ALLOCATOR);
        }
    }
}
