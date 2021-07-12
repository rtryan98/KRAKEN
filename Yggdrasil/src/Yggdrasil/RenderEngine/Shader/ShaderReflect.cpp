// Copyright 2021 Robert Ryan. See LICENCE.md.

#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/Shader/ShaderReflect.h"
#include "Yggdrasil/RenderEngine/GraphicsDevice.h"
#include "Yggdrasil/RenderEngine/Descriptor/DescriptorSetLayoutCache.h"

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <array>
#include <spirv-reflect/spirv_reflect.h>

namespace Ygg::ShaderReflect
{
    // spirv_cross::Compiler GetReflection(const std::vector<uint32_t>& spirv)
    // {
    //     return spirv_cross::Compiler(spirv);
    // }
    // 
    // VkShaderStageFlagBits ParseShaderStage(const spirv_cross::Compiler& reflection)
    // {
    //     switch (reflection.get_execution_model())
    //     {
    //     case spv::ExecutionModelVertex:
    //         return VK_SHADER_STAGE_VERTEX_BIT;
    //     case spv::ExecutionModelTessellationControl:
    //         return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    //     case spv::ExecutionModelTessellationEvaluation:
    //         return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    //     case spv::ExecutionModelGeometry:
    //         return VK_SHADER_STAGE_GEOMETRY_BIT;
    //     case spv::ExecutionModelFragment:
    //         return VK_SHADER_STAGE_FRAGMENT_BIT;
    //     case spv::ExecutionModelGLCompute:
    //         return VK_SHADER_STAGE_COMPUTE_BIT;
    //     case spv::ExecutionModelTaskNV:
    //         return VK_SHADER_STAGE_TASK_BIT_NV;
    //     case spv::ExecutionModelMeshNV:
    //         return VK_SHADER_STAGE_MESH_BIT_NV;
    //     case spv::ExecutionModelRayGenerationKHR:
    //         return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    //     case spv::ExecutionModelIntersectionKHR:
    //         return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
    //     case spv::ExecutionModelAnyHitKHR:
    //         return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
    //     case spv::ExecutionModelClosestHitKHR:
    //         return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    //     case spv::ExecutionModelMissKHR:
    //         return VK_SHADER_STAGE_MISS_BIT_KHR;
    //     case spv::ExecutionModelCallableKHR:
    //         return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    //     default:
    //         YGG_WARN("Unknown shader execution model.");
    //         return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    //     }
    // }
    // 
    // void ParseLocalGroupSize(const spirv_cross::Compiler& reflection, SProgram& program)
    // {
    //     program.localGroupSize.x = reflection.get_execution_mode_argument(spv::ExecutionModeLocalSize, 0);
    //     program.localGroupSize.y = reflection.get_execution_mode_argument(spv::ExecutionModeLocalSize, 1);
    //     program.localGroupSize.z = reflection.get_execution_mode_argument(spv::ExecutionModeLocalSize, 2);
    // }

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

    // The stageflags are ignored here because they wouldn't allow to combine the set layouts efficiently.
    struct SVkDescriptorSetLayoutBindingHash
    {
        std::size_t operator()(const VkDescriptorSetLayoutBinding& k) const
        {
            return static_cast<std::size_t>(k.binding) * ((k.descriptorCount << 2) ^ (k.descriptorType));
        }
    };

    struct SVkDescriptorSetLayoutBindingEqualityOperator
    {
        bool operator()(const VkDescriptorSetLayoutBinding& a, const VkDescriptorSetLayoutBinding& b) const
        {
            return (a.binding == b.binding)
                & (a.descriptorCount == b.descriptorCount)
                & (a.descriptorType == b.descriptorType)
                & (a.pImmutableSamplers == b.pImmutableSamplers);
        }
    };

    struct SVkPushConstantRangeHash
    {

    };

    struct SVkPushConstantEqualityOperator
    {

    };

    /// @brief 
    /// @param spv SPIRV code
    /// @param setMap Map of set of all bindings
    /// @param setBindingShaderStages Map of Maps representing shader stages. [setNumber][bindingNumber] = shaderStage
    void ParseStage(
        SShaderReflectionWrapper shader,
        std::unordered_map<uint32_t,std::unordered_set<VkDescriptorSetLayoutBinding,
                SVkDescriptorSetLayoutBindingHash,
                SVkDescriptorSetLayoutBindingEqualityOperator>>& setMap,
        std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>>& setBindingShaderStages)
    {
        SpvReflectShaderModule reflectModule{};
        if (spvReflectCreateShaderModule(static_cast<uint32_t>(shader.spirv.size()) * sizeof(uint32_t), shader.spirv.data(), &reflectModule) != SPV_REFLECT_RESULT_SUCCESS)
            YGG_WARN("Reflection of shader failed.");

        uint32_t count{ 0 };
        if (spvReflectEnumerateDescriptorSets(&reflectModule, &count, nullptr) != SPV_REFLECT_RESULT_SUCCESS)
            YGG_WARN("Reflection descriptor set enumeration failed.");

        std::vector<SpvReflectDescriptorSet*> sets(count);
        if (spvReflectEnumerateDescriptorSets(&reflectModule, &count, sets.data()) != SPV_REFLECT_RESULT_SUCCESS)
            YGG_WARN("Reflection descriptor set enumeration failed.");

        for (uint32_t set{ 0 }; set < sets.size(); set++)
        {
            const SpvReflectDescriptorSet& reflectionSet{ *(sets[set]) };

            if (setMap.find(reflectionSet.set) == setMap.end())
            {
                setMap[reflectionSet.set] = std::unordered_set<VkDescriptorSetLayoutBinding,
                        SVkDescriptorSetLayoutBindingHash,
                        SVkDescriptorSetLayoutBindingEqualityOperator>();
            }
            auto& currentSet{ setMap[reflectionSet.set] };
            currentSet.reserve(reflectionSet.binding_count);

            for (uint32_t bindingNumber{ 0 }; bindingNumber < reflectionSet.binding_count; bindingNumber++)
            {
                const SpvReflectDescriptorBinding& reflectionBinding{ *(reflectionSet.bindings[bindingNumber]) };

                VkDescriptorSetLayoutBinding resultBinding{};
                resultBinding.binding = reflectionBinding.binding;
                resultBinding.descriptorType = static_cast<VkDescriptorType>(reflectionBinding.descriptor_type);
                resultBinding.descriptorCount = 1;
                for (uint32_t dimension{ 0 }; dimension < reflectionBinding.array.dims_count; dimension++)
                {
                    resultBinding.descriptorCount *= reflectionBinding.array.dims[dimension];
                }

                currentSet.insert(resultBinding);

                if (setBindingShaderStages.find(reflectionBinding.set) == setBindingShaderStages.end())
                {
                    setBindingShaderStages[reflectionBinding.set] = std::unordered_map<uint32_t, uint32_t>();
                }

                setBindingShaderStages[reflectionBinding.set][reflectionBinding.binding] |= static_cast<VkShaderStageFlagBits>(reflectModule.shader_stage);
            }
        }

        if (reflectModule.entry_points != nullptr)
        {
            shader.shader.localGroupSize.x = reflectModule.entry_points->local_size.x;
            shader.shader.localGroupSize.y = reflectModule.entry_points->local_size.y;
            shader.shader.localGroupSize.z = reflectModule.entry_points->local_size.z;
        }
        shader.shader.stage = static_cast<VkShaderStageFlagBits>(reflectModule.shader_stage);

        spvReflectDestroyShaderModule(&reflectModule);
    }

    void ParseProgram(CDescriptorSetLayoutCache& cache, SProgramReflectionWrapper& program, const CGraphicsDevice& device)
    {
        std::unordered_map<uint32_t, std::unordered_set<
                VkDescriptorSetLayoutBinding,
                SVkDescriptorSetLayoutBindingHash,
                SVkDescriptorSetLayoutBindingEqualityOperator>> setMap{};
        std::unordered_map<uint32_t, std::unordered_map<
                uint32_t, uint32_t>> setBindingShaderStages{};

        for (const auto& shader : program.shaders)
        {
            ParseStage(shader, setMap, setBindingShaderStages);
        }

        for (const auto& set : setMap)
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings{};
            for (const auto& descriptorSetLayoutBinding : set.second)
            {
                VkDescriptorSetLayoutBinding binding = descriptorSetLayoutBinding;
                binding.stageFlags = setBindingShaderStages[set.first][binding.binding];
                bindings.push_back(binding);
            }
            VkDescriptorSetLayoutCreateInfo createInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            createInfo.pBindings = bindings.data();
            createInfo.flags = 0x0; // TODO: flags for sets, bindless

            VkDescriptorSetLayout layout{ cache.CreateDescriptorSetLayout(&createInfo) };
            program.program.setLayouts.push_back(layout);
        }

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(program.program.setLayouts.size());
        pipelineLayoutCreateInfo.pSetLayouts = program.program.setLayouts.data();
        // pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(program.program.pushConstantRanges.size());
        // pipelineLayoutCreateInfo.pPushConstantRanges = program.program.pushConstantRanges.data();
        program.program.pipelineLayout = device.CreatePipelineLayout(&pipelineLayoutCreateInfo);
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
