#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/graphics/Context.h"
#include "Yggdrasil/core/graphics/Globals.h"
#include "Yggdrasil/core/graphics/Util.h"
#include "Yggdrasil/Defines.h"
#include "Yggdrasil/Types.h"
#include "Yggdrasil/core/Globals.h"
#include "Yggdrasil/core/Application.h"
#include "Yggdrasil/core/util/Log.h"
#include <sstream>
#include <set>

namespace yggdrasil::graphics
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

    void createCommandPools(Context& context)
    {
        VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = context.device.queues.rasterizerQueueFamilyIndex;

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
        YGGDRASIL_CORE_TRACE("Creating Surface.");
        context.screen.createSurface(globals::APPLICATION->getWindow(), context.instance);
        YGGDRASIL_CORE_TRACE("Creating Device.");
        context.device.create(context.instance, context.screen);
        YGGDRASIL_CORE_TRACE("Creating Swapchain.");
        context.screen.createSwapchain(context.device);
        YGGDRASIL_CORE_TRACE("Creating Commandpools.");
        createCommandPools(context);
        YGGDRASIL_CORE_TRACE("Creating per-frame Sync-objects.");
        createSyncObjects(context);

        printGpuMemoryInfo(context);
    }
    
    void freeContext(Context& context)
    {
        VK_CHECK(vkDeviceWaitIdle(context.device.logical));

        freeSyncObjects(context);
        for (uint32_t i{ 0 }; i < context.screen.swapchainImages.size(); i++)
        {
            if (context.commandPools[i] != VK_NULL_HANDLE)
            {
                vkDestroyCommandPool(context.device.logical, context.commandPools[i], VK_CPU_ALLOCATOR);
            }
        }
        context.screen.freeSwapchainFramebuffers(context.device);
        context.screen.freeSwapchain(context.device);
        context.device.free();
        context.screen.freeSurface(context.instance);

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
