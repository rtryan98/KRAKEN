#include "KRAKEN/core/graphics/Renderer.h"
#include "KRAKEN/core/Globals.h"
#include <vector>
#include "KRAKEN/core/Application.h"
#include <map>
#include "KRAKEN/core/window/Window.h"

namespace kraken
{
    void Renderer::createCommandPool()
    {
        VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        createInfo.queueFamilyIndex = this->context.graphicsQueueIndex;
        VK_CHECK(vkCreateCommandPool(this->context.device, &createInfo, vulkan::VK_CPU_ALLOCATOR, &this->commandPool));
    }

    void Renderer::onUpdate()
    {
        vkDeviceWaitIdle(this->context.device);
        uint32_t imageIndex{};
        VK_CHECK(vkAcquireNextImageKHR(this->context.device, this->context.swapchain, ~0ull, this->acquireSemaphore, VK_NULL_HANDLE, &imageIndex));

        VK_CHECK(vkResetCommandPool(this->context.device, this->commandPool, 0x0));

        VkCommandBufferAllocateInfo commandBufferAllocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = this->commandPool;
        commandBufferAllocateInfo.commandBufferCount = 1;

        VK_CHECK(vkAllocateCommandBuffers(this->context.device, &commandBufferAllocateInfo, &commandBuffer));

        VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        // TODO: TEMPORARY
        VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = this->context.swapchainImages[imageIndex];
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        // TODO: TEMPORARY

        VK_CHECK(vkBeginCommandBuffer(this->commandBuffer, &commandBufferBeginInfo));
        // TODO: TEMPORARY
        vkCmdPipelineBarrier(this->commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        // TODO: TEMPORARY

        VkClearColorValue clearColor{ 1.0f, 0.5f, 0.0f, 1.0f };
        VkImageSubresourceRange range{};
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.levelCount = 1;
        range.layerCount = 1;
        // vkCmdClearColorImage(this->commandBuffer, this->swapchain.getSwapchainImages()[imageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &range);
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

        VK_CHECK(vkQueueSubmit(this->context.graphicsComputeQueue, 1, &submitInfo, VK_NULL_HANDLE));

        VkPresentInfoKHR queuePresentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        queuePresentInfo.swapchainCount = 1;
        queuePresentInfo.pSwapchains = &this->context.swapchain;
        queuePresentInfo.waitSemaphoreCount = 1;
        queuePresentInfo.pWaitSemaphores = &this->releaseSemaphore;
        queuePresentInfo.pImageIndices = &imageIndex;

        VK_CHECK(vkQueuePresentKHR(this->context.graphicsComputeQueue, &queuePresentInfo));
    }

    void Renderer::init(const Window& window)
    {
        KRAKEN_UNUSED_VARIABLE(window);
        vulkan::initContext(this->context);
        this->acquireSemaphore = vulkan::util::createSemaphore(this->context.device);
        this->releaseSemaphore = vulkan::util::createSemaphore(this->context.device);
        this->submitFence = vulkan::util::createFence(this->context.device);
        createCommandPool();
    }

    void Renderer::free()
    {
        VK_CHECK(vkDeviceWaitIdle(this->context.device));
        vkDestroyCommandPool(this->context.device, this->commandPool, vulkan::VK_CPU_ALLOCATOR);
        vkDestroyFence(this->context.device, this->submitFence, vulkan::VK_CPU_ALLOCATOR);
        vkDestroySemaphore(this->context.device, this->releaseSemaphore, vulkan::VK_CPU_ALLOCATOR);
        vkDestroySemaphore(this->context.device, this->acquireSemaphore, vulkan::VK_CPU_ALLOCATOR);
        vulkan::freeContext(this->context);
    }

    VkInstance Renderer::getInstance() const
    {
        return this->context.instance;
    }
}
