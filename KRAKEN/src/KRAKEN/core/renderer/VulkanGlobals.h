#pragma once
#include <vulkan/vulkan.h>

namespace kraken::vulkan
{
    /// Allocator Callback
    extern VkAllocationCallbacks* VK_CPU_ALLOCATOR;

    /// Instance, Device
    extern VkInstance INSTANCE;
    extern VkPhysicalDevice PHYSICAL_DEVICE;
    extern VkDevice DEVICE;

    /// Queues
    extern VkQueue GRAPHICS_QUEUE;
    extern VkQueue PRESENT_QUEUE;
    extern VkQueue COMPUTE_QUEUE;

    extern uint32_t GRAPHICS_QUEUE_INDEX;
    extern uint32_t PRESENT_QUEUE_INDEX;
    extern uint32_t COMPUTE_QUEUE_INDEX;
}
