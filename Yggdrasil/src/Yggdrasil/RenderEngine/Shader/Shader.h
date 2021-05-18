#pragma once
#include <vulkan/vulkan.h>

namespace Ygg
{
    constexpr static uint32_t MAX_DESCIRPTOR_SET_LAYOUTS_PER_SHADER{ 4 };

    struct SShader
    {
        VkShaderModule module;
        VkShaderStageFlagBits stage;
    };

    struct SProgram
    {
        VkPipelineBindPoint pipelineBindPoint;
        VkPipelineLayout pipelineLayout;

        VkDescriptorSetLayout descriptorSetLayout[MAX_DESCIRPTOR_SET_LAYOUTS_PER_SHADER]
        {
            VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE
        };
        VkDescriptorUpdateTemplate descriptorUpdateTemplate;

        VkShaderStageFlags pushConstantFlags;

        struct SLocalGroupSize
        {
            uint32_t x;
            uint32_t y;
            uint32_t z;
        } localGroupSize;
    };
}
