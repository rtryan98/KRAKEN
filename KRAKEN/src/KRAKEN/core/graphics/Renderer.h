#pragma once
#include <vulkan/vulkan.h>
#include "KRAKEN/core/graphics/vulkan/VulkanGlobals.h"
#include "KRAKEN/Defines.h"
#include <functional>
#include "KRAKEN/core/util/Log.h"
#include "KRAKEN/core/graphics/vulkan/VulkanUtils.h"
#include "KRAKEN/core/graphics/vulkan/VulkanDevice.h"
#include <vector>

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
        void createSwapchain(const Window& window);

    private:
        VkInstance instance{ VK_NULL_HANDLE };
        vulkan::Device device{};
        VkDebugUtilsMessengerEXT debugMessenger{ VK_NULL_HANDLE };
        VkSurfaceKHR surface{ VK_NULL_HANDLE };
        VkSwapchainKHR swapchain{ VK_NULL_HANDLE };
        std::vector<VkImage> swapchainImages{};
    };
}
