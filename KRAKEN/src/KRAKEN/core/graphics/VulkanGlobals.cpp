#include "KRAKEN/core/graphics/VulkanGlobals.h"

namespace kraken::vulkan
{
    /// Allocator Callback
    VkAllocationCallbacks* VK_CPU_ALLOCATOR{ nullptr };

    /// Instance, Device
    VkInstance INSTANCE{ 0 };
    VkPhysicalDevice PHYSICAL_DEVICE{ 0 };
    VkDevice DEVICE{ 0 };

    /// Queues
    VkQueue GRAPHICS_QUEUE{ 0 };
    VkQueue PRESENT_QUEUE{ 0 };
    VkQueue COMPUTE_QUEUE{ 0 };

    uint32_t GRAPHICS_QUEUE_INDEX{ 0 };
    uint32_t PRESENT_QUEUE_INDEX{ 0 };
    uint32_t COMPUTE_QUEUE_INDEX{ 0 };
}
