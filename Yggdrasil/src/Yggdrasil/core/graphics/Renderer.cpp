#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/graphics/Renderer.h"
#include "Yggdrasil/core/Globals.h"
#include "Yggdrasil/core/Application.h"
#include "Yggdrasil/core/window/Window.h"
#include "Yggdrasil/core/graphics/ShaderCompiler.h"
#include "Yggdrasil/core/graphics/vulkan/PipelineFactory.h"

#include <map>
#include <vector>

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
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        VK_CHECK(vkCreatePipelineLayout(this->context.device.logical, &pipelineLayoutCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->pipelineLayout));

        vulkan::GraphicsPipelineFactory factory{};
        factory.defaults(this->context);
        std::vector<uint32_t> fragment{ shadercompiler::compileGlsl("res/shader/main.frag") };
        factory.pushShaderStage(this->context.device.logical, fragment, VK_SHADER_STAGE_FRAGMENT_BIT);
        std::vector<uint32_t> vertex{ shadercompiler::compileGlsl("res/shader/main.vert") };
        factory.pushShaderStage(this->context.device.logical, vertex, VK_SHADER_STAGE_VERTEX_BIT);
        this->pipeline = factory.createPipeline(this->context.device.logical, this->renderPass, this->pipelineLayout);
        factory.clear(this->context.device.logical);
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
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = static_cast<float>(context.screen.swapchainImageExtent.height);
        viewport.width = static_cast<float>(context.screen.swapchainImageExtent.width);
        viewport.height = -static_cast<float>(context.screen.swapchainImageExtent.height);
        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;
        vkCmdSetViewport(this->perFrame.commandBuffer, 0, 1, &viewport);
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
