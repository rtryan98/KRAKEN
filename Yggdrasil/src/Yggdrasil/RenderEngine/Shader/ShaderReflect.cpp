// Copyright 2021 Robert Ryan. See LICENCE.md.

#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/Shader/ShaderReflect.h"
#include "Yggdrasil/RenderEngine/GraphicsDevice.h"

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <array>

namespace Ygg::ShaderReflect
{
    spirv_cross::Compiler GetReflection(const std::vector<uint32_t>& spirv)
    {
        return spirv_cross::Compiler(spirv);
    }

    void ParseShaderStage(const spirv_cross::Compiler& reflection, SShader& shader)
    {
        switch (reflection.get_execution_model())
        {
        case spv::ExecutionModelVertex:
            shader.stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case spv::ExecutionModelTessellationControl:
            shader.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            break;
        case spv::ExecutionModelTessellationEvaluation:
            shader.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            break;
        case spv::ExecutionModelGeometry:
            shader.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            break;
        case spv::ExecutionModelFragment:
            shader.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        case spv::ExecutionModelGLCompute:
            shader.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            break;
        case spv::ExecutionModelTaskNV:
            shader.stage = VK_SHADER_STAGE_TASK_BIT_NV;
            break;
        case spv::ExecutionModelMeshNV:
            shader.stage = VK_SHADER_STAGE_MESH_BIT_NV;
            break;
        case spv::ExecutionModelRayGenerationKHR:
            shader.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
            break;
        case spv::ExecutionModelIntersectionKHR:
            shader.stage = VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
            break;
        case spv::ExecutionModelAnyHitKHR:
            shader.stage = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
            break;
        case spv::ExecutionModelClosestHitKHR:
            shader.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
            break;
        case spv::ExecutionModelMissKHR:
            shader.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
            break;
        case spv::ExecutionModelCallableKHR:
            shader.stage = VK_SHADER_STAGE_CALLABLE_BIT_KHR;
            break;
        default:
            YGG_WARN("Unknown shader execution model.");
            shader.stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
            break;
        }
    }

    void ParseLocalGroupSize(const spirv_cross::Compiler& reflection, SProgram& program)
    {
        program.localGroupSize.x = reflection.get_execution_mode_argument(spv::ExecutionModeLocalSize, 0);
        program.localGroupSize.y = reflection.get_execution_mode_argument(spv::ExecutionModeLocalSize, 1);
        program.localGroupSize.z = reflection.get_execution_mode_argument(spv::ExecutionModeLocalSize, 2);
    }

    struct SShaderResource
    {
        uint32_t binding;
        VkDescriptorType type;

        bool operator==(const SShaderResource& other) const
        {
            return
                (this->binding == other.binding) &&
                (this->type == other.type);
        }
    };

    struct SShaderResourceHash
    {
        std::size_t operator()(const SShaderResource& resource) const
        {
            return static_cast<std::size_t>(resource.binding) * (static_cast<std::size_t > (resource.type) + 1ull);
        }
    };

    void Parse(
        const spirv_cross::Compiler& reflection,
        VkShaderStageFlags shaderStage,
        std::array<std::unordered_map<SShaderResource, VkShaderStageFlags, SShaderResourceHash>, 4>& setResources
        )
    {
        const auto& resources{ reflection.get_shader_resources() };

        auto PushResource
        { [&](SShaderResource& resource, uint32_t set)
            {
                auto location{ setResources[set].find(resource) };
                if (location != setResources[set].end())
                {
                    setResources[set][resource] |= shaderStage;
                }
                else
                {
                    setResources[set].insert(std::make_pair(resource, shaderStage));
                }
            }
        };

        for (const auto& spirvRes : resources.uniform_buffers)
        {
            SShaderResource res{};
            res.binding = reflection.get_decoration(spirvRes.id, spv::DecorationBinding);
            res.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            PushResource(res, reflection.get_decoration(spirvRes.id, spv::DecorationDescriptorSet));
        }

        for (const auto& spirvRes : resources.storage_buffers)
        {
            SShaderResource res{};
            res.binding = reflection.get_decoration(spirvRes.id, spv::DecorationBinding);
            res.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            PushResource(res, reflection.get_decoration(spirvRes.id, spv::DecorationDescriptorSet));
        }

        for (const auto& spirvRes : resources.sampled_images)
        {
            SShaderResource res{};
            res.binding = reflection.get_decoration(spirvRes.id, spv::DecorationBinding);
            res.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            PushResource(res, reflection.get_decoration(spirvRes.id, spv::DecorationDescriptorSet));
        }

        for (const auto& spirvRes : resources.storage_images)
        {
            SShaderResource res{};
            res.binding = reflection.get_decoration(spirvRes.id, spv::DecorationBinding);
            res.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

            // Texel buffers are handled differently from the other types.
            if (reflection.get_type(spirvRes.type_id).image.dim == spv::DimBuffer)
            {
                res.type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
            }

            PushResource(res, reflection.get_decoration(spirvRes.id, spv::DecorationDescriptorSet));
        }

        for (const auto& spirvRes : resources.separate_samplers)
        {
            SShaderResource res{};
            res.binding = reflection.get_decoration(spirvRes.id, spv::DecorationBinding);
            res.type = VK_DESCRIPTOR_TYPE_SAMPLER;
            PushResource(res, reflection.get_decoration(spirvRes.id, spv::DecorationDescriptorSet));
        }

        for (const auto& spirvRes : resources.separate_images)
        {
            SShaderResource res{};
            res.binding = reflection.get_decoration(spirvRes.id, spv::DecorationBinding);
            res.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

            // Texel buffers are handled differently from the other types.
            if (reflection.get_type(spirvRes.type_id).image.dim == spv::DimBuffer)
            {
                res.type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
            }

            PushResource(res, reflection.get_decoration(spirvRes.id, spv::DecorationDescriptorSet));
        }

        for (const auto& spirvRes : resources.acceleration_structures)
        {
            SShaderResource res{};
            res.binding = reflection.get_decoration(spirvRes.id, spv::DecorationBinding);
            res.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
            PushResource(res, reflection.get_decoration(spirvRes.id, spv::DecorationDescriptorSet));
        }

        // TODO: push constants
    }

    VkPipeline CreateGraphicsPipeline(
        const CGraphicsDevice& device,
        VkPipelineCache cache,
        VkRenderPass renderPass,
        std::initializer_list<const SShader*> shaders
        )
    {
        VkGraphicsPipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };

        std::vector<VkPipelineShaderStageCreateInfo> stages{};
        for (const SShader* shader : shaders)
        {
            VkPipelineShaderStageCreateInfo stageCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
            stageCreateInfo.stage = shader->stage;
            stageCreateInfo.pName = "main";
            stageCreateInfo.module = shader->module;
            stageCreateInfo.pSpecializationInfo = nullptr; // TODO: add specialization constants
            stages.push_back(stageCreateInfo);
        }
        createInfo.stageCount = static_cast<uint32_t>(stages.size());
        createInfo.pStages = stages.data();

        VkPipelineVertexInputStateCreateInfo vertexInputState{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        createInfo.pVertexInputState = &vertexInputState;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        createInfo.pInputAssemblyState = &inputAssemblyState;

        VkPipelineViewportStateCreateInfo viewportState{};
        createInfo.pViewportState = &viewportState;

        VkPipelineRasterizationStateCreateInfo rasterizationState{};

        VkPipelineMultisampleStateCreateInfo multisampleState{};

        VkPipelineDepthStencilStateCreateInfo depthStencilState{};

        VkPipelineColorBlendStateCreateInfo colorBlendState{};

        VkPipelineDynamicStateCreateInfo dynamicState{};

        createInfo.layout; // TODO: reflect

        createInfo.renderPass = renderPass;

        return device.CreateGraphicsPipeline(&createInfo, cache);
    }

    VkPipeline CreateComputePipeline(const CGraphicsDevice& device, VkPipelineCache cache, const SShader* shader)
    {
        VkComputePipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };

        createInfo.stage.module = shader->module;
        createInfo.stage.pName = "main";
        createInfo.stage.stage = shader->stage;
        createInfo.stage.pSpecializationInfo = nullptr; // TODO: add specialization constants
        createInfo.layout; // TODO: reflect

        return device.CreateComputePipeline(&createInfo, cache);
    }
}
