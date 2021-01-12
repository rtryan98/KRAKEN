#include "KRAKEN/core/graphics/Renderer.h"
#include "KRAKEN/core/Globals.h"
#include <vector>
#include "KRAKEN/core/Application.h"
#include <map>
#include "KRAKEN/core/window/Window.h"

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
        VK_CHECK(vkCreateInstance(&instanceCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->instance));
        KRAKEN_ASSERT_VALUE(this->instance);
    }

    void Renderer::setupDebugMessenger()
    {
        vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(this->instance, "vkCreateDebugUtilsMessengerEXT"));
        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(this->instance, "vkDestroyDebugUtilsMessengerEXT"));

        VkDebugUtilsMessengerCreateInfoEXT createInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;

        VK_CHECK(vkCreateDebugUtilsMessengerEXT(this->instance, &createInfo, vulkan::VK_CPU_ALLOCATOR, &this->debugMessenger));
        KRAKEN_ASSERT_VALUE(this->debugMessenger);
    }

    void Renderer::createSurface()
    {
        this->surface = globals::APPLICATION->getWindow()->getSurface(this->instance);
    }

    // TODO: cache for recreation
    void Renderer::createSwapchain(const Window& window)
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device.getPhysicalDevice(), this->surface, &surfaceCapabilities);

        VkSwapchainCreateInfoKHR swapchainCreateInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainCreateInfo.surface = this->surface;

        // Triple buffer if possible, double buffer elsewise, otherwise single buffer
        if (surfaceCapabilities.minImageCount >= 3)
        {
            swapchainCreateInfo.minImageCount = 3;
        }
        else if (surfaceCapabilities.minImageCount == 2)
        {
            swapchainCreateInfo.minImageCount = 2;
        }
        else
        {
            swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount;
        }

        // TODO: add support for HDR and other displays here
        swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapchainCreateInfo.imageExtent = window.getFramebufferSize();

        // TODO: we might want to write directly to the image later on
        if (surfaceCapabilities.supportedUsageFlags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        else
        {
            KRAKEN_CORE_CRITICAL("Swapchain does not support being written to.");
        }

        if (this->device.getPresentQueueIndex() != this->device.getGraphicsQueueIndex())
        {
            std::vector<uint32_t> familyIndices{ this->device.getPresentQueueIndex(), this->device.getGraphicsQueueIndex() };
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = familyIndices.data();
        }
        else
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        uint32_t presentModeCount{ 0 };
        vkGetPhysicalDeviceSurfacePresentModesKHR(this->device.getPhysicalDevice(), this->surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes( presentModeCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR(this->device.getPhysicalDevice(), this->surface, &presentModeCount, presentModes.data());

        bool_t mailboxAvailable{ false };
        for (VkPresentModeKHR presentMode : presentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                mailboxAvailable = true;
                break;
            }
        }

        if (mailboxAvailable)
        {
            swapchainCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        }
        else
        {
            swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        }

        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.imageArrayLayers = 1;

        uint32_t surfaceFormatCount{ 0 };
        vkGetPhysicalDeviceSurfaceFormatsKHR(this->device.getPhysicalDevice(), this->surface, &surfaceFormatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> availableFormats(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(this->device.getPhysicalDevice(), this->surface, &surfaceFormatCount, availableFormats.data());

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

        VK_CHECK(vkCreateSwapchainKHR(this->device.getDevice(), &swapchainCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->swapchain));
        KRAKEN_ASSERT_VALUE(this->swapchain);

        // uint32_t swapchainImageCount{ 0 };
        // vkGetSwapchainImagesKHR(this->device.getDevice(), this->swapchain, &swapchainImageCount, nullptr);
        // this->swapchainImages.reserve(swapchainImageCount);
        // vkGetSwapchainImagesKHR(this->device.getDevice(), this->swapchain, &swapchainImageCount, nullptr);
    }

    void Renderer::init(const Window& window)
    {
        createInstance();
#if KRAKEN_USE_ASSERTS
        setupDebugMessenger();
#endif
        createSurface();
        device.init(this->instance, this->surface);
        createSwapchain(window);
    }

    void Renderer::free()
    {
        vkDestroySwapchainKHR(this->device.getDevice(), this->swapchain, vulkan::VK_CPU_ALLOCATOR);
        device.free();
        vkDestroySurfaceKHR(this->instance, this->surface, vulkan::VK_CPU_ALLOCATOR);
#if KRAKEN_USE_ASSERTS
        vkDestroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, vulkan::VK_CPU_ALLOCATOR);
#endif
        vkDestroyInstance(this->instance, vulkan::VK_CPU_ALLOCATOR);
    }

    VkInstance Renderer::getInstance() const
    {
        return this->instance;
    }
}
