// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#include <vulkan/vulkan.h>

namespace Ygg
{
    constexpr static uint32_t MAX_DESCIRPTOR_SET_LAYOUTS_PER_SHADER{ 4 };

    struct SShader
    {
        VkShaderModule module;
        VkShaderStageFlagBits stage;

        struct SLocalGroupSize
        {
            uint32_t x;
            uint32_t y;
            uint32_t z;
        } localGroupSize;
    };

    struct SShaderReflectionWrapper
    {
        std::vector<uint32_t> spirv;
        SShader shader;
    };

    struct SProgram
    {
        VkPipeline pipeline;
        VkPipelineBindPoint pipelineBindPoint;
        VkPipelineLayout pipelineLayout;
        VkDescriptorUpdateTemplate descriptorUpdateTemplate;
        VkShaderStageFlags pushConstantFlags;

        std::vector<VkDescriptorSetLayout> setLayouts;
        std::vector<SShader> shaders;
        std::vector<VkPushConstantRange> pushConstantRanges;
    };

    struct SProgramReflectionWrapper
    {
        std::vector<SShaderReflectionWrapper> shaders;
        SProgram program;
    };
}
