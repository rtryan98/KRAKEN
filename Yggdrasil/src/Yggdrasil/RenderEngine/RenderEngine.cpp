// Copyright 2021 Robert Ryan. See LICENCE.md.

#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"
#include "Yggdrasil/RenderEngine/RenderUtil.h"
#include "Yggdrasil/RenderEngine/Shader/ShaderReflect.h"
#include "Yggdrasil/RenderEngine/Shader/ShaderCompiler.h"

#include <vulkan/vulkan.h>

namespace Ygg
{
    SProgram program{};

    void CRenderEngine::InitPerFrameStruct()
    {
        const auto& device{ this->m_context.GetGraphicsDevice() };
        const auto& screen{ this->m_context.GetScreen() };

        if (screen.GetData().swapchainImageCount >= 3)
        {
            YGG_TRACE("Resources will be triple buffered, resulting in a higher memory footprint.");
            this->m_maxFramesInFlight = 3;
        }
        else if (screen.GetData().swapchainImageCount == 2)
        {
            YGG_WARN("Triple buffering unavailable. Using double buffering.");
            this->m_maxFramesInFlight = 2;
        }
        else
        {
            YGG_CRITICAL("No supported buffering mode available! Single buffering will be used and will heavily impact performance.");
            this->m_currentFrameInFlight = 1;
        }

        for (uint32_t i{ 0 }; i < this->m_maxFramesInFlight; i++)
        {
            SPerFrame perFrame{};
            this->m_frames.push_back(perFrame);

            VkSemaphoreCreateInfo semaphoreInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
            semaphoreInfo.flags = 0;

            this->m_frames[i].acquireSemaphore = device.CreateSemaphore(
                &semaphoreInfo,
                std::string("Acquire Semaphore " + std::to_string(i)).c_str());
            PushObjectDeletion(
                [=]() -> void {
                    device.DestroySemaphore(&this->m_frames[i].acquireSemaphore);
                });

            this->m_frames[i].releaseSemaphore = device.CreateSemaphore(
                &semaphoreInfo,
                std::string("Release Semaphore " + std::to_string(i)).c_str());
            PushObjectDeletion(
                [=]() -> void {
                    device.DestroySemaphore(&this->m_frames[i].releaseSemaphore);
                });

            VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            this->m_frames[i].imageAcquireFence = device.CreateFence(
                &fenceInfo,
                std::string("Swapchain Image Fence " + std::to_string(i)).c_str()
            );
            PushObjectDeletion(
                [=]() -> void {
                    device.DestroyFence(&this->m_frames[i].imageAcquireFence);
                });

            VkCommandPoolCreateInfo cmdPoolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
            cmdPoolInfo.queueFamilyIndex = device.GetQueues().mainQueueFamilyIndex;

            this->m_frames[i].cmdPool = device.CreateCommandPool(
                &cmdPoolInfo,
                std::string("Frame Command Pool " + std::to_string(i)).c_str()
            );
            PushObjectDeletion(
                [=]() -> void {
                    device.DestroyCommandPool(&this->m_frames[i].cmdPool);
                });

            this->m_frames[i].cmdBuffer = device.AllocateCommandBuffer(
                this->m_frames[i].cmdPool,
                VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                std::string("Frame Command Buffer " + std::to_string(i)).c_str()
            );
        }
    }

    void CRenderEngine::Init()
    {
        this->m_context.Create();
        InitPerFrameStruct();
        ShaderCompiler::Init();
        this->m_descriptorSetLayoutCache.Init(this->m_context.GetGraphicsDevice());

        program = ShaderCompiler::CompileAndReflectShadersFromFiles(
            {"res/shader/test.vert", "res/shader/test.frag"}, this->m_descriptorSetLayoutCache, this->m_context.GetGraphicsDevice());

        VkGraphicsPipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };

        std::vector<VkPipelineShaderStageCreateInfo> stages{};
        for (const SShader& shader : program.shaders)
        {
            VkPipelineShaderStageCreateInfo stageCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
            stageCreateInfo.stage = shader.stage;
            stageCreateInfo.pName = "main";
            stageCreateInfo.module = shader.module;
            stageCreateInfo.pSpecializationInfo = nullptr; // TODO: add specialization constants
            stages.push_back(stageCreateInfo);
        }
        createInfo.stageCount = static_cast<uint32_t>(stages.size());
        createInfo.pStages = stages.data();

        VkPipelineVertexInputStateCreateInfo vertexInputState{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        createInfo.pVertexInputState = &vertexInputState;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        createInfo.pInputAssemblyState = &inputAssemblyState;

        VkPipelineViewportStateCreateInfo viewportState{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        VkViewport viewport{};
        viewport.width =  static_cast<float_t>(this->m_context.GetScreen().GetData().swapchainImageExtent.width);
        viewport.height = -static_cast<float_t>(this->m_context.GetScreen().GetData().swapchainImageExtent.height);
        viewport.y = static_cast<float_t>(this->m_context.GetScreen().GetData().swapchainImageExtent.height);
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        VkRect2D scissor{};
        scissor.extent = this->m_context.GetScreen().GetData().swapchainImageExtent;
        scissor.offset = {0, 0};
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
        createInfo.pViewportState = &viewportState;

        VkPipelineRasterizationStateCreateInfo rasterizationState{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizationState.depthClampEnable = VK_FALSE;
        rasterizationState.rasterizerDiscardEnable = VK_FALSE;
        rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationState.lineWidth = 1.0f;
        createInfo.pRasterizationState = &rasterizationState;

        VkPipelineMultisampleStateCreateInfo multisampleState{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        createInfo.pMultisampleState = &multisampleState;

        VkPipelineDepthStencilStateCreateInfo depthStencilState{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

        VkPipelineColorBlendStateCreateInfo colorBlendState{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
        colorBlendAttachmentState.blendEnable = VK_FALSE;
        colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        colorBlendState.attachmentCount = 1;
        colorBlendState.pAttachments = &colorBlendAttachmentState;
        colorBlendState.logicOpEnable = VK_FALSE;
        colorBlendState.logicOp = VK_LOGIC_OP_COPY;
        colorBlendState.blendConstants[0] = 0.0f;
        colorBlendState.blendConstants[1] = 0.0f;
        colorBlendState.blendConstants[2] = 0.0f;
        colorBlendState.blendConstants[3] = 0.0f;

        createInfo.pColorBlendState = &colorBlendState;

        createInfo.layout = program.pipelineLayout;

        createInfo.renderPass = this->m_context.GetScreen().GetData().swapchainRenderPass;
        program.pipeline = this->m_context.GetGraphicsDevice().CreateGraphicsPipeline(&createInfo);
    }

    void CRenderEngine::Render()
    {
        auto& device{ this->m_context.GetGraphicsDevice() };
        auto& screen{ this->m_context.GetScreen() };
        auto& frame { this->m_frames[m_currentFrameInFlight] };

        vkWaitForFences(device.GetHandle(), 1, &frame.imageAcquireFence, VK_TRUE, ~0ull);

        uint32_t imageIndex{ 0 };
        device.AcquireNextImageKHR(
            screen.GetData().swapchain, ~0ull, frame.acquireSemaphore, VK_NULL_HANDLE, &imageIndex);
        // TODO: handle resize

        VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        device.ResetCommandPool(frame.cmdPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
        RenderUtil::VkCheck(vkBeginCommandBuffer(frame.cmdBuffer, &beginInfo));

        screen.BeginSwapchainRenderPass(frame.cmdBuffer, imageIndex);

        vkCmdBindPipeline(frame.cmdBuffer, program.pipelineBindPoint, program.pipeline);
        vkCmdDraw(frame.cmdBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(frame.cmdBuffer);
        RenderUtil::VkCheck(vkEndCommandBuffer(frame.cmdBuffer));

        VkPipelineStageFlags submitStageMask{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &frame.acquireSemaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &frame.releaseSemaphore;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &frame.cmdBuffer;
        submitInfo.pWaitDstStageMask = &submitStageMask;

        device.ResetFence(frame.imageAcquireFence);
        RenderUtil::VkCheck(
            vkQueueSubmit(device.GetQueues().mainQueue, 1, &submitInfo, frame.imageAcquireFence));

        VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &screen.GetData().swapchain;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &frame.releaseSemaphore;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(device.GetQueues().mainQueue, &presentInfo);
        // TODO: handle resize

        this->m_currentFrameInFlight =
            (this->m_currentFrameInFlight + 1) % this->m_maxFramesInFlight;
    }

    void CRenderEngine::Shutdown()
    {
        this->m_context.GetGraphicsDevice().WaitIdle();
        ShaderCompiler::Free();
        this->m_descriptorSetLayoutCache.Destroy();
        for (auto it{ this->m_deletionQueue.rbegin() }; it != this->m_deletionQueue.rend(); it++)
        {
            (*it)();
        }
        this->m_context.Destroy();
    }

    void CRenderEngine::PushObjectDeletion(std::function<void()>&& mFunction)
    {
        this->m_deletionQueue.push_back(mFunction);
    }

    const CGraphicsContext& CRenderEngine::GetGraphicsContext() const
    {
        return this->m_context;
    }
}
