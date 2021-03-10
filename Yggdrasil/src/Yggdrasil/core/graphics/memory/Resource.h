#pragma once
#include <vulkan/vulkan.h>

namespace yggdrasil::graphics::memory
{
    struct AllocatedBuffer
    {
        VkBuffer buffer{};
        VkDeviceMemory memory{};
        uint32_t size{};
    };
}
