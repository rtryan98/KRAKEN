// Copyright 2021 Robert Ryan. See LICENCE.md.

#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/Shader/ShaderReflect.h"
#include "Yggdrasil/RenderEngine/GraphicsDevice.h"
#include "Yggdrasil/RenderEngine/Descriptor/DescriptorSetLayoutCache.h"

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <array>
#include <spirv_cross/spirv_reflect.hpp>

namespace Ygg::ShaderReflect
{
    spirv_cross::Compiler GetReflection(const std::vector<uint32_t>& spirv)
    {
        return spirv_cross::Compiler(spirv);
    }

    VkShaderStageFlagBits ParseShaderStage(const spirv_cross::Compiler& reflection)
    {
        switch (reflection.get_execution_model())
        {
        case spv::ExecutionModelVertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case spv::ExecutionModelTessellationControl:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case spv::ExecutionModelTessellationEvaluation:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case spv::ExecutionModelGeometry:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case spv::ExecutionModelFragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case spv::ExecutionModelGLCompute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        case spv::ExecutionModelTaskNV:
            return VK_SHADER_STAGE_TASK_BIT_NV;
        case spv::ExecutionModelMeshNV:
            return VK_SHADER_STAGE_MESH_BIT_NV;
        case spv::ExecutionModelRayGenerationKHR:
            return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        case spv::ExecutionModelIntersectionKHR:
            return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
        case spv::ExecutionModelAnyHitKHR:
            return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
        case spv::ExecutionModelClosestHitKHR:
            return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        case spv::ExecutionModelMissKHR:
            return VK_SHADER_STAGE_MISS_BIT_KHR;
        case spv::ExecutionModelCallableKHR:
            return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
        default:
            YGG_WARN("Unknown shader execution model.");
            return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
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
        uint32_t count;
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
            return static_cast<std::size_t>(resource.binding) * ((static_cast<std::size_t> (resource.type) + 1ull) << 2) * (resource.count);
        }
    };

    void ParseStage(
        const spirv_cross::Compiler& reflection,
        std::array<std::unordered_map<SShaderResource, VkShaderStageFlags, SShaderResourceHash>, 4>& setResources,
        SShader& shader
        )
    {
        const auto& resources{ reflection.get_shader_resources() };
        const auto shaderStage{ ParseShaderStage(reflection) };
        shader.stage = shaderStage;

        auto PushResource
        { [&](SShaderResource& resource, uint32_t set) -> void
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

        auto GetResourceCount
        { [&](SShaderResource& resource, const spirv_cross::SmallVector<uint32_t>& resourceArray) -> void
            {
                if (resourceArray.empty())
                {
                    resource.count = 1;
                }
                else
                {
                    resource.count = resourceArray[0];
                }
                // TODO: handle arrays of arrays?
            }
        };

        for (const auto& spirvRes : resources.uniform_buffers)
        {
            SShaderResource res{};
            res.binding = reflection.get_decoration(spirvRes.id, spv::DecorationBinding);
            res.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            GetResourceCount(res, reflection.get_type(spirvRes.id).array);
            PushResource(res, reflection.get_decoration(spirvRes.id, spv::DecorationDescriptorSet));
        }

        for (const auto& spirvRes : resources.storage_buffers)
        {
            SShaderResource res{};
            res.binding = reflection.get_decoration(spirvRes.id, spv::DecorationBinding);
            res.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            GetResourceCount(res, reflection.get_type(spirvRes.id).array);
            PushResource(res, reflection.get_decoration(spirvRes.id, spv::DecorationDescriptorSet));
        }

        for (const auto& spirvRes : resources.sampled_images)
        {
            SShaderResource res{};
            res.binding = reflection.get_decoration(spirvRes.id, spv::DecorationBinding);
            res.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            GetResourceCount(res, reflection.get_type(spirvRes.id).array);
            PushResource(res, reflection.get_decoration(spirvRes.id, spv::DecorationDescriptorSet));
        }

        for (const auto& spirvRes : resources.storage_images)
        {
            SShaderResource res{};
            res.binding = reflection.get_decoration(spirvRes.id, spv::DecorationBinding);
            res.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            GetResourceCount(res, reflection.get_type(spirvRes.id).array);

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
            GetResourceCount(res, reflection.get_type(spirvRes.id).array);
            PushResource(res, reflection.get_decoration(spirvRes.id, spv::DecorationDescriptorSet));
        }

        for (const auto& spirvRes : resources.separate_images)
        {
            SShaderResource res{};
            res.binding = reflection.get_decoration(spirvRes.id, spv::DecorationBinding);
            res.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            GetResourceCount(res, reflection.get_type(spirvRes.id).array);

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
            GetResourceCount(res, reflection.get_type(spirvRes.id).array);

            PushResource(res, reflection.get_decoration(spirvRes.id, spv::DecorationDescriptorSet));
        }

        // TODO: push constants
    }

    SProgram ParseProgram(
        std::initializer_list<std::reference_wrapper<SShaderWrapper>> shaders,
        CDescriptorSetLayoutCache& setLayoutCache,
        const CGraphicsDevice& device)
    {
        std::array<std::unordered_map<SShaderResource, VkShaderStageFlags, SShaderResourceHash>, 4> setResources{};
        for (const auto& shaderWrapperReference : shaders)
        {
            const auto spirvShaderReflection{ GetReflection(shaderWrapperReference.get().spirv) };
            ParseStage(spirvShaderReflection, setResources, shaderWrapperReference.get().shader);
        }

        std::vector<VkDescriptorSetLayout> setLayouts{};
        for (const auto& resourceMap : setResources)
        {
            std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
            for (const auto& resource : resourceMap)
            {
                setLayoutBindings.push_back(
                    {
                        resource.first.binding,
                        resource.first.type,
                        resource.first.count,
                        resource.second,
                        nullptr
                    });
            }

            VkDescriptorSetLayoutCreateInfo setLayoutCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            setLayoutCreateInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
            setLayoutCreateInfo.pBindings = setLayoutBindings.data();
            setLayoutCreateInfo.flags = 0x0; // TODO: bindless?

            setLayouts.push_back(setLayoutCache.CreateDescriptorSetLayout(&setLayoutCreateInfo));
        }

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutCreateInfo.pSetLayouts = setLayouts.data();
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0; // TODO: push constants
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // TODO: push constants
        pipelineLayoutCreateInfo.flags = 0x0;

        // TODO: IMPORTANT: Create Cache for PipelineLayout
        SProgram program{};

        VkPipelineLayout pipelineLayout{ device.CreatePipelineLayout(&pipelineLayoutCreateInfo) };
        program.pipelineLayout = pipelineLayout;

        // TODO: derive update template
        program.descriptorUpdateTemplate = VK_NULL_HANDLE;

        // TODO: derive push constant flags
        program.pushConstantFlags = 0x0;

        // TODO: parse local group size
        program.localGroupSize = {0, 0, 0};

        program.descriptorSetLayout[0] = VK_NULL_HANDLE;
        program.descriptorSetLayout[1] = VK_NULL_HANDLE;
        program.descriptorSetLayout[2] = VK_NULL_HANDLE;
        program.descriptorSetLayout[3] = VK_NULL_HANDLE;

        for (uint32_t i{ 0 }; i < setLayouts.size(); i++)
        {
            program.descriptorSetLayout[i] = setLayouts[i];
        }

        return program;
    }

    VkPipeline CreateGraphicsPipeline(
        const CGraphicsDevice& device,
        VkPipelineCache cache,
        VkRenderPass renderPass,
        std::initializer_list<const SShader*> shaders,
        const SProgram* pProgram
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

        VkPipelineViewportStateCreateInfo viewportState{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        createInfo.pViewportState = &viewportState;

        VkPipelineRasterizationStateCreateInfo rasterizationState{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        createInfo.pRasterizationState = &rasterizationState;

        VkPipelineMultisampleStateCreateInfo multisampleState{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };

        VkPipelineDepthStencilStateCreateInfo depthStencilState{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

        VkPipelineColorBlendStateCreateInfo colorBlendState{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };

        VkPipelineDynamicStateCreateInfo dynamicState{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.dynamicStateCount = 2;
        VkDynamicState dynamicStates[2]
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        dynamicState.pDynamicStates = dynamicStates;
        createInfo.pDynamicState = &dynamicState;

        createInfo.layout = pProgram->pipelineLayout;

        createInfo.renderPass = renderPass;

        return device.CreateGraphicsPipeline(&createInfo, cache);
    }

    VkPipeline CreateComputePipeline(
        const CGraphicsDevice& device,
        VkPipelineCache cache,
        const SShader* pShader,
        const SProgram* pProgram)
    {
        VkComputePipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };

        createInfo.stage.module = pShader->module;
        createInfo.stage.pName = "main";
        createInfo.stage.stage = pShader->stage;
        createInfo.stage.pSpecializationInfo = nullptr; // TODO: add specialization constants
        createInfo.layout = pProgram->pipelineLayout;

        return device.CreateComputePipeline(&createInfo, cache);
    }
}
