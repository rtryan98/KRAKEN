#pragma once
#include <vulkan/vulkan.h>

namespace yggdrasil::graphics
{
    class Device;
}

namespace yggdrasil::graphics::memory
{
    uint32_t getMemoryTypeIndex(const Device& device, VkMemoryPropertyFlags flags);
}
