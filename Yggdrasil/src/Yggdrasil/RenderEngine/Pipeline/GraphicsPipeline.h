#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace Ygg
{
    struct SGraphicsPipelineInfo
    {
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos{};
        VkPipelineVertexInputStateCreateInfo         vertexInputStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        VkPipelineInputAssemblyStateCreateInfo       inputAssemblyStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        VkPipelineViewportStateCreateInfo            viewportStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        VkPipelineRasterizationStateCreateInfo       rasterizationStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        VkPipelineColorBlendStateCreateInfo          colorBlendStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        VkPipelineMultisampleStateCreateInfo         multisampleStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        VkPipelineLayoutCreateInfo                   pipelineLayoutCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        VkPipelineTessellationStateCreateInfo        tesselationStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
        VkPipelineDepthStencilStateCreateInfo        depthStencilStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        VkPipelineDynamicStateCreateInfo             dynamicStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        uint32_t                                     pipelineCreateFlags{};
        VkViewport                                   viewport{};
        VkRect2D                                     scissor{};
        VkPipelineColorBlendAttachmentState          colorBlendAttachmentState{};
        std::vector<VkDynamicState>                  dynamicStates{};
    };
}
