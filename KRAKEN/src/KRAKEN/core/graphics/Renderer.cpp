#include "KRAKEN/pch.h"

#include "KRAKEN/core/graphics/Renderer.h"
#include "KRAKEN/core/Globals.h"
#include <vector>
#include "KRAKEN/core/Application.h"
#include <map>
#include "KRAKEN/core/window/Window.h"

namespace kraken
{
    void Renderer::createRenderPasses()
    {
        VkAttachmentDescription attachmentDescription{};
        attachmentDescription.format = this->context.swapchainImageFormat;
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentReference{};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;

        VkRenderPassCreateInfo renderPassCreateInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &attachmentDescription;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 0;
        renderPassCreateInfo.pDependencies = nullptr;

        VK_CHECK(vkCreateRenderPass(this->context.device.logical, &renderPassCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->renderPass));
    }

    void Renderer::createFramebuffers()
    {
        this->framebuffers.resize(this->context.swapchainImageViews.size());

        VkFramebufferCreateInfo framebufferCreateInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        framebufferCreateInfo.renderPass = this->renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.width = this->context.swapchainImageExtent.width;
        framebufferCreateInfo.height = this->context.swapchainImageExtent.height;
        framebufferCreateInfo.layers = 1;

        for (uint32_t i{0}; i < this->context.swapchainImageViews.size(); i++)
        {
            framebufferCreateInfo.pAttachments = &this->context.swapchainImageViews[i];
            VK_CHECK(vkCreateFramebuffer(this->context.device.logical, &framebufferCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->framebuffers[i]));
        }
    }

    void Renderer::createShaderModules()
    {
        std::vector<char> vertSPIRV{ vulkan::util::parseSPIRV("res/shader/vert.spv") };
        VkShaderModuleCreateInfo vertCreateInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        vertCreateInfo.codeSize = vertSPIRV.size();
        vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertSPIRV.data());
        vkCreateShaderModule(this->context.device.logical, &vertCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->vert);
        KRAKEN_ASSERT_VALUE(this->vert);

        std::vector<char> fragSPIRV{ vulkan::util::parseSPIRV("res/shader/frag.spv") };
        VkShaderModuleCreateInfo fragCreateInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        fragCreateInfo.codeSize = fragSPIRV.size();
        fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragSPIRV.data());
        vkCreateShaderModule(this->context.device.logical, &fragCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->frag);
        KRAKEN_ASSERT_VALUE(this->frag);
    }

    void Renderer::createPipeline()
    {
        VkPipelineShaderStageCreateInfo vertStageCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        vertStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStageCreateInfo.module = this->vert;
        vertStageCreateInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragStageCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        fragStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStageCreateInfo.module = this->frag;
        fragStageCreateInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStageCreateInfos[2]{ vertStageCreateInfo, fragStageCreateInfo };

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
        inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(this->context.swapchainImageExtent.width);
        viewport.height = static_cast<float>(this->context.swapchainImageExtent.height);
        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;

        VkRect2D scissor{};
        scissor.extent = this->context.swapchainImageExtent;
        scissor.offset = { 0, 0 };

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pViewports = &viewport;
        viewportStateCreateInfo.scissorCount = 1;
        viewportStateCreateInfo.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizerCreateInfo.depthClampEnable = VK_FALSE;
        rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizerCreateInfo.lineWidth = 1.0f;
        rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizerCreateInfo.depthBiasClamp = 0.0f;
        rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
        rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleCreateInfo.minSampleShading = 1.0f;
        multisampleCreateInfo.pSampleMask = nullptr;
        multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendState{};
        colorBlendState.blendEnable = VK_FALSE;
        colorBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendState.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendState.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlendStateCreateInfo.attachmentCount = 1;
        colorBlendStateCreateInfo.pAttachments = &colorBlendState;
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
        colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
        colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
        colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

        VkDynamicState dynamicStates[]{
            VK_DYNAMIC_STATE_VIEWPORT
        };

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicStateCreateInfo.dynamicStateCount = 1;
        dynamicStateCreateInfo.pDynamicStates = dynamicStates;

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

        VK_CHECK(vkCreatePipelineLayout(this->context.device.logical, &pipelineLayoutCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->pipelineLayout));

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

        VkGraphicsPipelineCreateInfo pipelineCreateInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipelineCreateInfo.stageCount = 2;
        pipelineCreateInfo.pStages = shaderStageCreateInfos;
        pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
        pipelineCreateInfo.pTessellationState = nullptr;
        pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
        pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
        pipelineCreateInfo.pDepthStencilState = nullptr;
        pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
        pipelineCreateInfo.pDynamicState = nullptr;
        pipelineCreateInfo.layout = this->pipelineLayout;
        pipelineCreateInfo.renderPass = this->renderPass;
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;

        VK_CHECK(vkCreateGraphicsPipelines(this->context.device.logical, VK_NULL_HANDLE, 1, &pipelineCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->pipeline));
    }

    void Renderer::onUpdate()
    {
        uint32_t imageIndex{};
        VK_CHECK(vkAcquireNextImageKHR(this->context.device.logical, this->context.swapchain, ~0ull, this->acquireSemaphore, VK_NULL_HANDLE, &imageIndex));



        VK_CHECK(vkResetCommandPool(this->context.device.logical, this->context.commandPool, 0x0));

        VkCommandBufferAllocateInfo commandBufferAllocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = this->context.commandPool;
        commandBufferAllocateInfo.commandBufferCount = 1;

        VK_CHECK(vkAllocateCommandBuffers(this->context.device.logical, &commandBufferAllocateInfo, &commandBuffer));

        VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(this->commandBuffer, &commandBufferBeginInfo));

        VkClearValue clearValue{};
        clearValue.color = { 0.1f, 0.1f, 0.1f, 1.0f };

        VkRenderPassBeginInfo renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        renderPassBeginInfo.framebuffer = this->framebuffers[imageIndex];
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;
        renderPassBeginInfo.renderPass = this->renderPass;
        renderPassBeginInfo.renderArea.extent = this->context.swapchainImageExtent;
        renderPassBeginInfo.renderArea.offset = {0, 0};

        vkCmdBeginRenderPass(this->commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(this->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);
        vkCmdDraw(this->commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(this->commandBuffer);

        VK_CHECK(vkEndCommandBuffer(this->commandBuffer));

        VkPipelineStageFlags submitStageMask{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &this->acquireSemaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &this->releaseSemaphore;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &this->commandBuffer;
        submitInfo.pWaitDstStageMask = &submitStageMask;

        VK_CHECK(vkQueueSubmit(this->context.queues.rasterizerQueue, 1, &submitInfo, VK_NULL_HANDLE));

        VkPresentInfoKHR queuePresentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        queuePresentInfo.swapchainCount = 1;
        queuePresentInfo.pSwapchains = &this->context.swapchain;
        queuePresentInfo.waitSemaphoreCount = 1;
        queuePresentInfo.pWaitSemaphores = &this->releaseSemaphore;
        queuePresentInfo.pImageIndices = &imageIndex;

        VK_CHECK(vkQueuePresentKHR(this->context.queues.presentQueue, &queuePresentInfo));
        VK_CHECK(vkQueueWaitIdle(this->context.queues.rasterizerQueue));
    }

    void Renderer::createSyncObjects()
    {
        this->acquireSemaphore = vulkan::util::createSemaphore(this->context.device.logical);
        this->releaseSemaphore = vulkan::util::createSemaphore(this->context.device.logical);
    }

    void Renderer::freeSyncObjects()
    {
        vkDestroySemaphore(this->context.device.logical, this->acquireSemaphore, vulkan::VK_CPU_ALLOCATOR);
        vkDestroySemaphore(this->context.device.logical, this->releaseSemaphore, vulkan::VK_CPU_ALLOCATOR);
    }

    void Renderer::init(const Window& window)
    {
        KRAKEN_UNUSED_VARIABLE(window);
        vulkan::initContext(this->context);
        createRenderPasses();
        createFramebuffers();
        createSyncObjects();
        createShaderModules();
        createPipeline();
    }

    void Renderer::free()
    {
        VK_CHECK(vkDeviceWaitIdle(this->context.device.logical));

        if (this->pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(this->context.device.logical, this->pipeline, vulkan::VK_CPU_ALLOCATOR);
        }
        if (this->pipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(this->context.device.logical, this->pipelineLayout, vulkan::VK_CPU_ALLOCATOR);
        }
        if (this->frag != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(this->context.device.logical, this->frag, vulkan::VK_CPU_ALLOCATOR);
        }
        if (this->vert != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(this->context.device.logical, this->vert, vulkan::VK_CPU_ALLOCATOR);
        }
        freeSyncObjects();
        for (uint32_t i{ 0 }; i < this->framebuffers.size(); i++)
        {
            if (this->framebuffers[i] != VK_NULL_HANDLE)
            {
                vkDestroyFramebuffer(this->context.device.logical, this->framebuffers[i], vulkan::VK_CPU_ALLOCATOR);
            }
        }
        if (this->renderPass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(this->context.device.logical, this->renderPass, vulkan::VK_CPU_ALLOCATOR);
        }
        vulkan::freeContext(this->context);
    }

    const vulkan::Context& Renderer::getContext() const
    {
        return this->context;
    }
}
