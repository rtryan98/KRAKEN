// Copyright 2021 Robert Ryan. See LICENCE.md.

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
