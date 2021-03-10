#pragma once
#include "Yggdrasil/core/graphics/Screen.h"
#include <vulkan/vulkan.h>

namespace yggdrasil::graphics
{
    class Device
    {
    public:
        void create(VkInstance instance, Screen& screen);
        void free();

        VkPhysicalDevice physical{};
        VkDevice logical{};

        struct Queues
        {
            // queues
            VkQueue rasterizerQueue{};
            VkQueue asyncComputeQueue{};
            VkQueue transferQueue{};
            VkQueue presentQueue{};

            // queue family indices                        // Different functions on each chip.
            uint32_t rasterizerQueueFamilyIndex{};         // Main rasterizer queue does synchronous compute, graphics and maybe present
            uint32_t transferQueueFamilyIndex{};           // Does async transfer if exists
            uint32_t asyncComputeQueueFamilyIndex{};       // Does async compute if exists
            uint32_t presentQueueFamilyIndex{};            // Does present either on async compute or graphics depending on where post process is done
        } queues;

        struct Memory
        {
            VkPhysicalDeviceMemoryProperties properties{};
        } memory;
    };
}
