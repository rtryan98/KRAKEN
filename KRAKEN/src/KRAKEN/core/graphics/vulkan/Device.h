#pragma once
#include <vulkan/vulkan.h>
#include "KRAKEN/Types.h"

namespace kraken::vulkan
{
    class Device
    {
    public:
        Device() = default;
        ~Device() = default;

        void init(VkInstance instance, VkSurfaceKHR surface);
        void free();

        VkPhysicalDevice getPhysicalDevice() const;
        VkDevice getDevice() const;
        VkQueue getGraphicsQueue() const;
        VkQueue getComputeQueue() const;
        VkQueue getPresentQueue() const;
        uint32_t getGraphicsQueueIndex() const;
        uint32_t getComputeQueueIndex() const;
        uint32_t getPresentQueueIndex() const;

    private:
        void selectPhysicalDevice(VkInstance instance);
        void logPhysicalDeviceInfo();
        void findQueueFamilies(VkSurfaceKHR surface);
        void createDevice();

    private:
        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
        VkDevice device{ VK_NULL_HANDLE };

        VkQueue graphicsQueue{ VK_NULL_HANDLE };
        VkQueue computeQueue{ VK_NULL_HANDLE };
        VkQueue presentQueue{ VK_NULL_HANDLE };

        uint32_t graphicsQueueIndex{ 0 };
        uint32_t computeQueueIndex{ 0 };
        uint32_t presentQueueIndex{ 0 };
    };
}
