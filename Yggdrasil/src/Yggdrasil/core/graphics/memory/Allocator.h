#pragma once
#include <vulkan/vulkan.h>

namespace ygg::graphics
{
    class Device;
}

namespace ygg::graphics::memory
{
    uint32_t getMemoryTypeIndex(const Device& device,
        VkMemoryPropertyFlags required,
        VkMemoryPropertyFlags excluded);
}
