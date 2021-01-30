#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace kraken::vulkan
{
    struct Context
    {
        VkInstance instance;

        VkDebugUtilsMessengerEXT debugMessenger;

        VkPhysicalDevice physicalDevice;
        VkDevice device;

        uint32_t presentQueueIndex;
        uint32_t computeQueueIndex;
        uint32_t graphicsQueueIndex;

        VkQueue graphicsComputeQueue;
        VkQueue presentQueue;

        VkSurfaceKHR surface;
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        VkFormat swapchainImageFormat;
    };

    void initContext(Context& context);

    void freeContext(Context& context);
}
