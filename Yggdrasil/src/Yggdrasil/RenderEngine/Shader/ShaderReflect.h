// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#include "Yggdrasil/RenderEngine/Shader/Shader.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <initializer_list>

namespace Ygg
{
    class CGraphicsDevice;
    class CDescriptorSetLayoutCache;
}

namespace Ygg::ShaderReflect
{
    struct SShaderWrapper
    {
        std::vector<uint32_t> spirv;
        SShader shader{};
    };

    SProgram ParseProgram(
        std::initializer_list<std::reference_wrapper<SShaderWrapper>> shaders,
        CDescriptorSetLayoutCache& setLayoutCache,
        const CGraphicsDevice& device);

    // TODO: move that somewhere else?
    VkPipeline CreateGraphicsPipeline(
        const CGraphicsDevice& device,
        VkPipelineCache cache,
        VkRenderPass renderPass,
        std::initializer_list<const SShader*> shaders,
        const SProgram* pProgram
        );

    // TODO: move that somewhere else?
    VkPipeline CreateComputePipeline(
        const CGraphicsDevice& device,
        VkPipelineCache cache,
        const SShader* pShader,
        const SProgram* pProgram
        );
}
