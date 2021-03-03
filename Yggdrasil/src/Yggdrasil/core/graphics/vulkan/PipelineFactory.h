#pragma once
#include "Yggdrasil/core/graphics/vulkan/Context.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <queue>

namespace yggdrasil::vulkan
{
    class GraphicsPipelineFactory
    {
    public:
        /// <summary>
        /// Creates a VkPipeline based on the prior set data.
        /// The returned handle must be externally deleted once the program terminates.
        /// </summary>
        /// <param name="context">The Context which the application uses.</param>
        /// <param name="renderPass">The renderpass for which to create this pipeline.</param>
        /// <param name="pipelineLayout">The pipelineLayout of this pipeline.</param>
        /// <returns> The created VkPipeline handle </returns>
        VkPipeline createPipeline(Context& context, VkRenderPass renderPass, VkPipelineLayout pipelineLayout);

        /// <summary>
        /// Clears the pipeline factory.
        /// This also destroys any shader module created with it.
        /// Sets all createinfos into their default state.
        /// </summary>
        /// <param name="context">The Context which the application uses.</param>
        void clear(Context& context);

        /// <summary>
        /// Sets the pipeline factory into default state.
        /// This also clears the pipeline factory prior to that action.
        /// </summary>
        /// <param name="context">The Context which the application uses.</param>
        void defaults(Context& context);

        /// <summary>
        /// Pushes a shader stage which will be used in the generated pipeline.
        /// </summary>
        /// <param name="context">The Context which the application uses.</param>
        /// <param name="spirv">The compiled code.</param>
        /// <param name="flags">The shader stage.</param>
        void pushShaderStage(Context& context, std::vector<uint32_t>& spirv, VkShaderStageFlagBits flags );

        /// <summary>
        /// Resets all shader stages pushed into the pipeline factory.
        /// Also clears all shader modules created.
        /// </summary>
        /// <param name="context">The Context which the application uses.</param>
        void resetShaderStages(Context& context);

        VkPipelineVertexInputStateCreateInfo&        getVertexInputStateCreateInfo();
        VkPipelineInputAssemblyStateCreateInfo&      getInputAssemblyStateCreateInfo();
        VkPipelineViewportStateCreateInfo&           getViewportStateCreateInfo();
        VkPipelineRasterizationStateCreateInfo&      getRasterizationStateCreateInfo();
        VkPipelineColorBlendStateCreateInfo&         getColorBlendStateCreateInfo();
        VkPipelineMultisampleStateCreateInfo&        getMultisampleStateCreateInfo();
        VkPipelineLayoutCreateInfo&                  getLayoutCreateInfo();
        VkPipelineTessellationStateCreateInfo&       getTessellationStateCreateInfo();
        VkPipelineDepthStencilStateCreateInfo&       getDepthStencilStateCreateInfo();
        VkPipelineDynamicStateCreateInfo&            getDynamicStateCreateInfo();
        std::vector<VkDynamicState>                  getDynamicStates();

    private:
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos{};
        VkPipelineVertexInputStateCreateInfo         vertexInputStateCreateInfo   { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO   };
        VkPipelineInputAssemblyStateCreateInfo       inputAssemblyStateCreateInfo { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        VkPipelineViewportStateCreateInfo            viewportStateCreateInfo      { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO       };
        VkPipelineRasterizationStateCreateInfo       rasterizationStateCreateInfo { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO  };
        VkPipelineColorBlendStateCreateInfo          colorBlendStateCreateInfo    { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO    };
        VkPipelineMultisampleStateCreateInfo         multisampleStateCreateInfo   { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO    };
        VkPipelineLayoutCreateInfo                   pipelineLayoutCreateInfo     { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO               };
        VkPipelineTessellationStateCreateInfo        tesselationStateCreateInfo   { VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO   };
        VkPipelineDepthStencilStateCreateInfo        depthStencilStateCreateInfo  { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO  };
        VkPipelineDynamicStateCreateInfo             dynamicStateCreateInfo       { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO        };
        uint32_t                                     pipelineCreateFlags{};
        VkViewport                                   viewport{};
        VkRect2D                                     scissor{};
        VkPipelineColorBlendAttachmentState          colorBlendAttachmentState{};
        std::vector<VkDynamicState>                  dynamicStates{};

        std::queue<VkShaderModule>                   shaderModuleDeletionQueue{};
    };
}
