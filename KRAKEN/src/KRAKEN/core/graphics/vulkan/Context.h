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

        struct Queues
        {
            // queues
            VkQueue mainRasterizerQueue;
            VkQueue asyncComputeQueue;
            VkQueue directMemoryAccessQueue;
            VkQueue presentQueue;

            // queue family indices                      // Different functions on each chip.
            uint32_t mainRasterizerQueueFamilyIndex;     // Main rasterizer queue does synchronous compute, graphics and maybe present
            uint32_t directMemoryAccessQueueFamilyIndex; // Does async compute if exists
            uint32_t asyncComputeQueueFamilyIndex;       // Does async transfer if exists
            uint32_t presentQueueFamilyIndex;            // Does present async if seperate queue, synchronous with other queue otherwise
        } queues;

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
