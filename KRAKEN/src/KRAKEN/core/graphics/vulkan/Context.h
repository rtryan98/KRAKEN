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

        // uint32_t presentQueueIndex;
        // uint32_t graphicsComputeQueueIndex;
        uint32_t graphicsComputePresentQueueFamilyIndex;

        // VkQueue graphicsComputeQueue;
        // VkQueue presentQueue;
        VkQueue graphicsComputePresentQueue;

        VkSurfaceKHR surface;
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        VkFormat swapchainImageFormat;
        VkExtent2D swapchainImageExtent;

        VkCommandPool commandPool;
    };

    void initContext(Context& context);

    void freeContext(Context& context);
}
