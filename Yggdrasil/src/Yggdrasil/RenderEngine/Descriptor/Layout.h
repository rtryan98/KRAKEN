#pragma once

#include <vulkan/vulkan.h>

namespace Ygg
{
    struct SDescriptorSetLayoutInfo
    {
        uint32_t set;
        VkDescriptorSetLayout layout;
    };
}
