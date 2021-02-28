#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/graphics/Renderer.h"
#include "Yggdrasil/core/Globals.h"
#include <vector>
#include "Yggdrasil/core/Application.h"
#include <map>
#include "Yggdrasil/core/window/Window.h"

namespace yggdrasil
{
    void Renderer::createRenderPasses()
    {
        VkAttachmentDescription attachmentDescription{};
        attachmentDescription.format = this->context.screen.swapchainImageFormat;
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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
        this->framebuffers.resize(this->context.screen.swapchainImageViews.size());

        VkFramebufferCreateInfo framebufferCreateInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        framebufferCreateInfo.renderPass = this->renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.width = this->context.screen.swapchainImageExtent.width;
        framebufferCreateInfo.height = this->context.screen.swapchainImageExtent.height;
        framebufferCreateInfo.layers = 1;

        for (uint32_t i{0}; i < this->context.screen.swapchainImageViews.size(); i++)
        {
            framebufferCreateInfo.pAttachments = &this->context.screen.swapchainImageViews[i];
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
        YGGDRASIL_ASSERT_VALUE(this->vert);

        std::vector<char> fragSPIRV{ vulkan::util::parseSPIRV("res/shader/frag.spv") };
        VkShaderModuleCreateInfo fragCreateInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        fragCreateInfo.codeSize = fragSPIRV.size();
        fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragSPIRV.data());
        vkCreateShaderModule(this->context.device.logical, &fragCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->frag);
        YGGDRASIL_ASSERT_VALUE(this->frag);
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
        viewport.y = static_cast<float>(this->context.screen.swapchainImageExtent.height);
        viewport.width = static_cast<float>(this->context.screen.swapchainImageExtent.width);
        viewport.height = -static_cast<float>(this->context.screen.swapchainImageExtent.height);
        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;

        VkRect2D scissor{};
        scissor.extent = this->context.screen.swapchainImageExtent;
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
        rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

    void Renderer::acquirePerFrameData()
    {
        this->perFrame.acquireSemaphore = this->context.syncObjects.acquireSemaphores[this->perFrame.frame];
        this->perFrame.releaseSemaphore = this->context.syncObjects.releaseSemaphores[this->perFrame.frame];
        this->perFrame.acquireFence = this->context.syncObjects.acquireFences[this->perFrame.frame];

        vkWaitForFences(this->context.device.logical, 1, &this->perFrame.acquireFence, VK_TRUE, ~0ull);

        uint32_t imageIndex{};
        VK_CHECK(vkAcquireNextImageKHR(this->context.device.logical, this->context.screen.swapchain, ~0ull, this->perFrame.acquireSemaphore, VK_NULL_HANDLE, &imageIndex));
        this->currentImage = imageIndex;
        this->perFrame.commandPool = this->context.commandPools[imageIndex];
        this->perFrame.commandBuffer = this->context.commandBuffers[imageIndex];
        this->perFrame.swapchainImage = this->context.screen.swapchainImages[imageIndex];
        this->perFrame.swapchainImageView = this->context.screen.swapchainImageViews[imageIndex];
        this->perFrame.framebuffer = this->framebuffers[imageIndex];
    }

    void Renderer::prepare()
    {
        acquirePerFrameData();
        VK_CHECK(vkResetCommandPool(this->context.device.logical, this->perFrame.commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
        VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK(vkBeginCommandBuffer(this->perFrame.commandBuffer, &commandBufferBeginInfo));
    }

    void Renderer::onUpdate()
    {
        VkClearValue clearValue{};
        clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };

        VkRenderPassBeginInfo renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        renderPassBeginInfo.framebuffer = this->perFrame.framebuffer;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;
        renderPassBeginInfo.renderPass = this->renderPass;
        renderPassBeginInfo.renderArea.extent = this->context.screen.swapchainImageExtent;
        renderPassBeginInfo.renderArea.offset = {0, 0};
        vkCmdBeginRenderPass(this->perFrame.commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(this->perFrame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);
        vkCmdDraw(this->perFrame.commandBuffer, 3, 1, 0, 0);
        vkCmdEndRenderPass(this->perFrame.commandBuffer);
    }

    void Renderer::present()
    {
        VkImageMemoryBarrier layoutBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        layoutBarrier.image = this->perFrame.swapchainImage;
        layoutBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        layoutBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        layoutBarrier.srcQueueFamilyIndex = this->context.queues.rasterizerQueueFamilyIndex;
        layoutBarrier.dstQueueFamilyIndex = this->context.queues.rasterizerQueueFamilyIndex;
        layoutBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        layoutBarrier.subresourceRange.layerCount = 1;
        layoutBarrier.subresourceRange.baseArrayLayer = 0;
        layoutBarrier.subresourceRange.levelCount = 1;
        layoutBarrier.subresourceRange.baseMipLevel = 0;

        vkCmdPipelineBarrier(this->perFrame.commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,
            0, nullptr, 0, nullptr, 1, &layoutBarrier);

        VK_CHECK(vkEndCommandBuffer(this->perFrame.commandBuffer));
        VkPipelineStageFlags submitStageMask{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &this->perFrame.acquireSemaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &this->perFrame.releaseSemaphore;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &this->perFrame.commandBuffer;
        submitInfo.pWaitDstStageMask = &submitStageMask;

        VK_CHECK(vkResetFences(this->context.device.logical, 1, &this->perFrame.acquireFence));
        VK_CHECK(vkQueueSubmit(this->context.queues.rasterizerQueue, 1, &submitInfo, this->perFrame.acquireFence));

        VkPresentInfoKHR queuePresentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        queuePresentInfo.swapchainCount = 1;
        queuePresentInfo.pSwapchains = &this->context.screen.swapchain;
        queuePresentInfo.waitSemaphoreCount = 1;
        queuePresentInfo.pWaitSemaphores = &this->perFrame.releaseSemaphore;
        queuePresentInfo.pImageIndices = &this->currentImage;

        VK_CHECK(vkQueuePresentKHR(this->context.queues.presentQueue, &queuePresentInfo));
        this->perFrame.frame = (this->perFrame.frame + 1) % static_cast<uint32_t>(this->context.screen.swapchainImages.size());
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
        YGGDRASIL_UNUSED_VARIABLE(window);
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

    VkRenderPass Renderer::getRenderPass() const
    {
        return this->renderPass;
    }

    const PerFrame& Renderer::getPerFrameData() const
    {
        return this->perFrame;
    }

    const std::vector<VkFramebuffer>& Renderer::getFramebuffers() const
    {
        return this->framebuffers;
    }

}
