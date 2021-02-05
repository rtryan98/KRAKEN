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

        VK_CHECK(vkCreateRenderPass(this->context.device, &renderPassCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->renderPass));
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
            VK_CHECK(vkCreateFramebuffer(this->context.device, &framebufferCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->framebuffers[i]));
        }
    }

    void Renderer::onUpdate()
    {
        // TODO: TEMPORARY
        vkDeviceWaitIdle(this->context.device);
        // TODO: TEMPORARY

        uint32_t imageIndex{};
        VK_CHECK(vkAcquireNextImageKHR(this->context.device, this->context.swapchain, ~0ull, this->acquireSemaphore, VK_NULL_HANDLE, &imageIndex));

        VK_CHECK(vkResetCommandPool(this->context.device, this->context.commandPool, 0x0));

        VkCommandBufferAllocateInfo commandBufferAllocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = this->context.commandPool;
        commandBufferAllocateInfo.commandBufferCount = 1;

        VK_CHECK(vkAllocateCommandBuffers(this->context.device, &commandBufferAllocateInfo, &commandBuffer));

        VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(this->commandBuffer, &commandBufferBeginInfo));

        VkClearValue clearValue{};
        clearValue.color = { 1.0f, 0.5f, 0.0f, 1.0f };

        VkRenderPassBeginInfo renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        renderPassBeginInfo.framebuffer = this->framebuffers[imageIndex];
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;
        renderPassBeginInfo.renderPass = this->renderPass;
        renderPassBeginInfo.renderArea.extent = this->context.swapchainImageExtent;
        renderPassBeginInfo.renderArea.offset = {0, 0};


        vkCmdBeginRenderPass(this->commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

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

        VK_CHECK(vkQueueSubmit(this->context.graphicsComputePresentQueue, 1, &submitInfo, VK_NULL_HANDLE));

        VkPresentInfoKHR queuePresentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        queuePresentInfo.swapchainCount = 1;
        queuePresentInfo.pSwapchains = &this->context.swapchain;
        queuePresentInfo.waitSemaphoreCount = 1;
        queuePresentInfo.pWaitSemaphores = &this->releaseSemaphore;
        queuePresentInfo.pImageIndices = &imageIndex;

        VK_CHECK(vkQueuePresentKHR(this->context.graphicsComputePresentQueue, &queuePresentInfo));
    }

    void Renderer::init(const Window& window)
    {
        KRAKEN_UNUSED_VARIABLE(window);
        vulkan::initContext(this->context);
        this->acquireSemaphore = vulkan::util::createSemaphore(this->context.device);
        this->releaseSemaphore = vulkan::util::createSemaphore(this->context.device);
        this->submitFence = vulkan::util::createFence(this->context.device);
        createRenderPasses();
        createFramebuffers();
    }

    void Renderer::free()
    {
        VK_CHECK(vkDeviceWaitIdle(this->context.device));

        for (uint32_t i{ 0 }; i < this->framebuffers.size(); i++)
        {
            vkDestroyFramebuffer(this->context.device, this->framebuffers[i], vulkan::VK_CPU_ALLOCATOR);
        }
        vkDestroyRenderPass(this->context.device, this->renderPass, vulkan::VK_CPU_ALLOCATOR);
        vkDestroyFence(this->context.device, this->submitFence, vulkan::VK_CPU_ALLOCATOR);
        vkDestroySemaphore(this->context.device, this->releaseSemaphore, vulkan::VK_CPU_ALLOCATOR);
        vkDestroySemaphore(this->context.device, this->acquireSemaphore, vulkan::VK_CPU_ALLOCATOR);
        vulkan::freeContext(this->context);
    }

    const vulkan::Context& Renderer::getContext() const
    {
        return this->context;
    }
}
