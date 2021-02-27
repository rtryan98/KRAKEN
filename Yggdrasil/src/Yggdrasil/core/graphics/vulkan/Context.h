#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace yggdrasil::vulkan
{
    struct Context
    {
        VkInstance instance;

        VkDebugUtilsMessengerEXT debugMessenger;

        struct Device
        {
            VkPhysicalDevice physical;
            VkDevice logical;
        } device;

        struct Queues
        {
            // queues
            VkQueue rasterizerQueue;
            VkQueue asyncComputeQueue;
            VkQueue transferQueue;
            VkQueue presentQueue;

            // queue family indices                      // Different functions on each chip.
            uint32_t rasterizerQueueFamilyIndex;         // Main rasterizer queue does synchronous compute, graphics and maybe present
            uint32_t transferQueueFamilyIndex;           // Does async transfer if exists
            uint32_t asyncComputeQueueFamilyIndex;       // Does async compute if exists
            uint32_t presentQueueFamilyIndex;            // Does present either on async compute or graphics depending on where post process is done
        } queues;

        struct Screen
        {
            VkSurfaceKHR surface;
            VkSwapchainKHR swapchain;
            std::vector<VkImage> swapchainImages;
            std::vector<VkImageView> swapchainImageViews;
            VkFormat swapchainImageFormat;
            VkExtent2D swapchainImageExtent;
        } screen;

        uint32_t maxFramesInFlight{ 2 };

        std::vector<VkCommandPool> commandPools{};
        std::vector<VkCommandBuffer> commandBuffers{};

        struct Sync
        {
            std::vector<VkSemaphore> acquireSemaphores{};
            std::vector<VkSemaphore> releaseSemaphores{};
            std::vector<VkFence> acquireFences{};
        } syncObjects;
    };

    void initContext(Context& context);

    void freeContext(Context& context);
}
