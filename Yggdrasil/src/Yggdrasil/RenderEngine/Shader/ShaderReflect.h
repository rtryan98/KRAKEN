// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#include "Yggdrasil/RenderEngine/Shader/Shader.h"

#include <spirv_cross/spirv_reflect.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <initializer_list>

namespace Ygg
{
    class CGraphicsDevice;
}

namespace Ygg::ShaderReflect
{
    spirv_cross::Compiler GetReflection(const std::vector<uint32_t>& spirv);

    // TODO: move that somewhere else?
    VkPipeline CreateGraphicsPipeline(
        const CGraphicsDevice& device,
        VkPipelineCache cache,
        VkRenderPass renderPass,
        std::initializer_list<const SShader*> shaders
        );

    // TODO: move that somewhere else?
    VkPipeline CreateComputePipeline(
        const CGraphicsDevice& device,
        VkPipelineCache cache,
        const SShader* shader
        );
}
