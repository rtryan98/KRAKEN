#pragma once
#include <vulkan/vulkan.h>
#include "KRAKEN/core/renderer/VulkanGlobals.h"
#include "KRAKEN/Defines.h"
#include <functional>
#include "KRAKEN/core/util/Log.h"

#if KRAKEN_USE_ASSERTS
    #define VK_CHECK(fun)                               \
    do {                                                \
        VkResult vkCheckResult{ fun };                  \
        KRAKEN_ASSERT( vkCheckResult == VK_SUCCESS );   \
    } while( 0 )
#else
    #define VK_CHECK(fun) fun
#endif

namespace kraken
{
    class Renderer
    {
    public:
        Renderer() = default;
        ~Renderer() = default;

        void init();
        void free();

    private:
        void createInstance();
        void setupDebugMessenger();
        void selectPhysicalDevice();
        void createSurface();
        void findQueueFamilies();
        void createDevice();
        void logPhysicalDeviceInfo();

    private:
        VkDebugUtilsMessengerEXT debugMessenger{};
        VkSurfaceKHR surface{};
    };
}
