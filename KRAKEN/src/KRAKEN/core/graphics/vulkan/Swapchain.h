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
        void getSwapchainImages(uint32_t imageCount, const Device& device);
        void createImageViews(const Device& device);

    private:
        VkSwapchainKHR swapchain{ VK_NULL_HANDLE };
        VkFormat format{};
        std::vector<VkImage> swapchainImages{};
        std::vector<VkImageView> swapchainImageViews{};
    };
}
