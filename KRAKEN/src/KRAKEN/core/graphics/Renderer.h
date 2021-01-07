#pragma once
#include <vulkan/vulkan.h>
#include "KRAKEN/core/graphics/vulkan/VulkanGlobals.h"
#include "KRAKEN/Defines.h"
#include <functional>
#include "KRAKEN/core/util/Log.h"
#include "KRAKEN/core/graphics/vulkan/VulkanUtils.h"

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
