#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"
#include "Yggdrasil/RenderEngine/RenderUtil.h"

#include <vulkan/vulkan.h>

namespace Ygg
{
    void CRenderEngine::InitPerFrameStruct()
    {
        auto& device{ this->m_context.GetGraphicsDeviceNonConst() };
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
            device.PushObjectDeletion(
                [=]() mutable -> void {
                    device.DestroySemaphore(&this->m_frames[i].acquireSemaphore);
                });

            this->m_frames[i].releaseSemaphore = device.CreateSemaphore(
                &semaphoreInfo,
                std::string("Release Semaphore " + std::to_string(i)).c_str());
            device.PushObjectDeletion(
                [=]() mutable -> void {
                    device.DestroySemaphore(&this->m_frames[i].releaseSemaphore);
                });

            VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            this->m_frames[i].imageAcquireFence = device.CreateFence(
                &fenceInfo,
                std::string("Swapchain Image Fence " + std::to_string(i)).c_str()
            );
            device.PushObjectDeletion(
                [=]() mutable -> void {
                    device.DestroyFence(&this->m_frames[i].imageAcquireFence);
                });

            VkCommandPoolCreateInfo cmdPoolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
            cmdPoolInfo.queueFamilyIndex = device.GetQueues().mainQueueFamilyIndex;

            this->m_frames[i].cmdPool = device.CreateCommandPool(
                &cmdPoolInfo,
                std::string("Frame Command Pool " + std::to_string(i)).c_str()
            );
            device.PushObjectDeletion(
                [=]() mutable -> void {
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
    }

    void CRenderEngine::Render()
    {
        auto& device{ this->m_context.GetGraphicsDeviceNonConst() };
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
        this->m_context.Destroy();
    }

    const CGraphicsContext& CRenderEngine::GetGraphicsContext() const
    {
        return this->m_context;
    }
}
