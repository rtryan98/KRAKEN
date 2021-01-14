#pragma once
#include <vulkan/vulkan.h>
#include "KRAKEN/core/graphics/vulkan/Globals.h"
#include "KRAKEN/Defines.h"
#include <functional>
#include "KRAKEN/core/util/Log.h"
#include "KRAKEN/core/graphics/vulkan/Util.h"
#include "KRAKEN/core/graphics/vulkan/Device.h"
#include "KRAKEN/core/graphics/vulkan/Swapchain.h"

namespace kraken
{
    class Window;

    class Renderer
    {
    public:
        Renderer() = default;
        ~Renderer() = default;

        void init(const Window& window);
        void free();

        VkInstance getInstance() const;

    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();

    private:
        VkInstance instance{ VK_NULL_HANDLE };
        vulkan::Device device{};
        VkDebugUtilsMessengerEXT debugMessenger{ VK_NULL_HANDLE };
        VkSurfaceKHR surface{ VK_NULL_HANDLE };
        vulkan::Swapchain swapchain{};
    };
}
