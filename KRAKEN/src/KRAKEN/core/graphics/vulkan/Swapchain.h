#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "KRAKEN/core/graphics/vulkan/Device.h"
#include "KRAKEN/core/window/Window.h"

namespace kraken::vulkan
{
    class Swapchain
    {
    public:
        Swapchain() = default;
        ~Swapchain() = default;

        void init(VkSurfaceKHR surface, const Window& window, const Device& device);
        void free(const Device& device);

        VkSwapchainKHR getSwapchain() const;
    private:

        VkSwapchainKHR swapchain{ VK_NULL_HANDLE };
        std::vector<VkImage> swapchainImages{};
    };
}
