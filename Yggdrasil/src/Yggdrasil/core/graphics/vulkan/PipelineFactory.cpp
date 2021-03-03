#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/graphics/vulkan/PipelineFactory.h"
#include "Yggdrasil/core/graphics/vulkan/Util.h"
#include "Yggdrasil/core/graphics/vulkan/Globals.h"

namespace yggdrasil::vulkan
{
    void GraphicsPipelineFactory::clear(VkDevice device)
    {
        this->shaderStageCreateInfos.clear();
        this->dynamicStates.clear();
        this->vertexInputStateCreateInfo   = VkPipelineVertexInputStateCreateInfo   { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO   };
        this->inputAssemblyStateCreateInfo = VkPipelineInputAssemblyStateCreateInfo { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        this->viewportStateCreateInfo      = VkPipelineViewportStateCreateInfo      { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO       };
        this->rasterizationStateCreateInfo = VkPipelineRasterizationStateCreateInfo { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO  };
        this->colorBlendStateCreateInfo    = VkPipelineColorBlendStateCreateInfo    { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO    };
        this->multisampleStateCreateInfo   = VkPipelineMultisampleStateCreateInfo   { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO    };
        this->pipelineLayoutCreateInfo     = VkPipelineLayoutCreateInfo             { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO               };
        this->tesselationStateCreateInfo   = VkPipelineTessellationStateCreateInfo  { VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO   };
        this->depthStencilStateCreateInfo  = VkPipelineDepthStencilStateCreateInfo  { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO  };
        this->dynamicStateCreateInfo       = VkPipelineDynamicStateCreateInfo       { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO        };
        this->pipelineCreateFlags          = 0;
        this->viewport                     = VkViewport{};
        this->scissor                      = VkRect2D{};

        while (!this->shaderModuleDeletionQueue.empty())
        {
            vkDestroyShaderModule(device, shaderModuleDeletionQueue.front(), VK_CPU_ALLOCATOR);
            shaderModuleDeletionQueue.pop();
        }
    }

    void GraphicsPipelineFactory::pushShaderStage(VkDevice device, std::vector<uint32_t>& spirv, VkShaderStageFlagBits flags)
    {
        // TODO: move shader module creation somewhere else.
        VkShaderModuleCreateInfo shaderModuleCreateInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        shaderModuleCreateInfo.codeSize = sizeof(uint32_t) * spirv.size();
        shaderModuleCreateInfo.pCode = spirv.data();

        VkShaderModule module{};
        VK_CHECK( vkCreateShaderModule(device, &shaderModuleCreateInfo, VK_CPU_ALLOCATOR, &module) );
        YGGDRASIL_ASSERT_VALUE( module );
        this->shaderModuleDeletionQueue.push( module );

        VkPipelineShaderStageCreateInfo createInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        createInfo.module = module;
        createInfo.pName = "main";
        createInfo.stage = flags;

        this->shaderStageCreateInfos.push_back(createInfo);
    }

    void GraphicsPipelineFactory::resetShaderStages(VkDevice device)
    {
        this->shaderStageCreateInfos.clear();
        while (!this->shaderModuleDeletionQueue.empty())
        {
            vkDestroyShaderModule(device, shaderModuleDeletionQueue.front(), VK_CPU_ALLOCATOR);
            shaderModuleDeletionQueue.pop();
        }
    }

    void GraphicsPipelineFactory::defaults(Context& context)
    {
        clear(context.device.logical);

        this->inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
        this->inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        this->viewport.x = 0.0f;
        this->viewport.y = static_cast<float>(context.screen.swapchainImageExtent.height);
        this->viewport.width = static_cast<float>(context.screen.swapchainImageExtent.width);
        this->viewport.height = -static_cast<float>(context.screen.swapchainImageExtent.height);
        this->viewport.maxDepth = 1.0f;
        this->viewport.minDepth = 0.0f;

        this->scissor.extent = context.screen.swapchainImageExtent;
        this->scissor.offset = { 0, 0 };

        this->viewportStateCreateInfo.viewportCount = 1;
        this->viewportStateCreateInfo.pViewports = &viewport;
        this->viewportStateCreateInfo.scissorCount = 1;
        this->viewportStateCreateInfo.pScissors = &scissor;

        this->rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        this->rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        this->rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        this->rasterizationStateCreateInfo.lineWidth = 1.0f;
        this->rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        this->rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        this->rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
        this->rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        this->rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
        this->rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

        this->multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        this->multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        this->multisampleStateCreateInfo.minSampleShading = 1.0f;
        this->multisampleStateCreateInfo.pSampleMask = nullptr;
        this->multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
        this->multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

        this->colorBlendAttachmentState.blendEnable = VK_FALSE;
        this->colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        this->colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        this->colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        this->colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        this->colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        this->colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        this->colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        this->colorBlendStateCreateInfo.attachmentCount = 1;
        this->colorBlendStateCreateInfo.pAttachments = &this->colorBlendAttachmentState;
        this->colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        this->colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        this->colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
        this->colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
        this->colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
        this->colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

        this->dynamicStates.push_back( VK_DYNAMIC_STATE_VIEWPORT );
        this->dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(this->dynamicStates.size());
        this->dynamicStateCreateInfo.pDynamicStates = this->dynamicStates.data();

        this->vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
        this->vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
        this->vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
        this->vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;
    }

    VkPipeline GraphicsPipelineFactory::createPipeline(VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout)
    {
        VkGraphicsPipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        createInfo.flags               = this->pipelineCreateFlags;
        createInfo.stageCount          = static_cast<uint32_t>(this->shaderStageCreateInfos.size());
        createInfo.pStages             = this->shaderStageCreateInfos.data();
        createInfo.pVertexInputState   = &this->vertexInputStateCreateInfo;
        createInfo.pInputAssemblyState = &this->inputAssemblyStateCreateInfo;
        createInfo.pTessellationState  = &this->tesselationStateCreateInfo;
        createInfo.pViewportState      = &this->viewportStateCreateInfo;
        createInfo.pRasterizationState = &this->rasterizationStateCreateInfo;
        createInfo.pMultisampleState   = &this->multisampleStateCreateInfo;
        createInfo.pDepthStencilState  = &this->depthStencilStateCreateInfo;
        createInfo.pColorBlendState    = &this->colorBlendStateCreateInfo;
        createInfo.pDynamicState       = &this->dynamicStateCreateInfo;
        createInfo.layout              = pipelineLayout;
        createInfo.renderPass          = renderPass;
        createInfo.subpass             = 0;
        createInfo.basePipelineHandle  = VK_NULL_HANDLE;
        createInfo.basePipelineIndex   = -1;

        VkPipeline result{};
        VK_CHECK( vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, VK_CPU_ALLOCATOR, &result) );
        return result;
    }

    VkPipelineVertexInputStateCreateInfo& GraphicsPipelineFactory::getVertexInputStateCreateInfo()
    {
        return this->vertexInputStateCreateInfo;
    }

    VkPipelineInputAssemblyStateCreateInfo& GraphicsPipelineFactory::getInputAssemblyStateCreateInfo()
    {
        return this->inputAssemblyStateCreateInfo;
    }

    VkPipelineViewportStateCreateInfo& GraphicsPipelineFactory::getViewportStateCreateInfo()
    {
        return this->viewportStateCreateInfo;
    }

    VkPipelineRasterizationStateCreateInfo& GraphicsPipelineFactory::getRasterizationStateCreateInfo()
    {
        return this->rasterizationStateCreateInfo;
    }

    VkPipelineColorBlendStateCreateInfo& GraphicsPipelineFactory::getColorBlendStateCreateInfo()
    {
        return this->colorBlendStateCreateInfo;
    }

    VkPipelineMultisampleStateCreateInfo& GraphicsPipelineFactory::getMultisampleStateCreateInfo()
    {
        return this->multisampleStateCreateInfo;
    }

    VkPipelineLayoutCreateInfo& GraphicsPipelineFactory::getLayoutCreateInfo()
    {
        return this->pipelineLayoutCreateInfo;
    }

    VkPipelineTessellationStateCreateInfo& GraphicsPipelineFactory::getTessellationStateCreateInfo()
    {
        return this->tesselationStateCreateInfo;
    }

    VkPipelineDepthStencilStateCreateInfo& GraphicsPipelineFactory::getDepthStencilStateCreateInfo()
    {
        return this->depthStencilStateCreateInfo;
    }

    VkPipelineDynamicStateCreateInfo& GraphicsPipelineFactory::getDynamicStateCreateInfo()
    {
        return this->dynamicStateCreateInfo;
    }

    std::vector<VkDynamicState> GraphicsPipelineFactory::getDynamicStates()
    {
        return this->dynamicStates;
    }
}
