#pragma once

#include <vulkan/vulkan.h>
#include <deque>
#include <functional>

namespace Ygg
{
    struct GraphicsContext;

    struct GraphicsDevice
    {
        VkDevice handle;

        struct Queues
        {
            VkQueue mainQueue;
            uint32_t mainQueueFamilyIndex;

            VkQueue asyncComputeQueue;
            uint32_t asyncComputeQueueFamilyIndex;

            VkQueue copyQueue;
            uint32_t copyQueueFamilyIndex;
        } queues;

        struct GPU
        {
            VkPhysicalDevice handle;
            VkPhysicalDeviceProperties vulkan10Properties;
            VkPhysicalDeviceVulkan11Properties vulkan11Properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES };
            VkPhysicalDeviceVulkan12Properties vulkan12Properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES };
            VkPhysicalDeviceMemoryProperties memoryProperties;

            // VK_KHR_surface
            bool GetSurfaceSupportKHR(uint32_t queueFamilyIndex, VkSurfaceKHR surface);
            void GetSurfacePresentModesKHR(VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes);
            void GetSurfaceFormatsKHR(VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats);
            VkSurfaceCapabilitiesKHR GetSurfaceCapabilitiesKHR(VkSurfaceKHR surface);
        } gpu;

        VkPhysicalDeviceFeatures2 enabledVulkan10Features{};
        VkPhysicalDeviceVulkan11Features enabledVulkan11Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
        VkPhysicalDeviceVulkan12Features enabledVulkan12Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };

        void Create(GraphicsContext* pContext,
            bool enableAllAvailableFeatures = false,
            VkPhysicalDeviceFeatures* pRequestedFeatures = nullptr,
            VkPhysicalDeviceVulkan11Features* pRequestedVulkan11Features = nullptr,
            VkPhysicalDeviceVulkan12Features* pRequestedVulkan12Features = nullptr);
        void Destroy();

        void PushObjectDeletion(const std::function<void()>&& mFunction);

        VkCommandPool CreateCommandPool(VkCommandPoolCreateInfo* pCreateInfo);
        void DestroyCommandPool(VkCommandPool* pPool);

        VkBuffer CreateBuffer(VkBufferCreateInfo* pCreateInfo);
        void DestroyBuffer(VkBuffer* pBuffer);

        VkBufferView CreateBufferView(VkBufferViewCreateInfo* pCreateInfo);
        void DestroyBufferView(VkBufferView* pBufferView);

        VkImage CreateImage(VkImageCreateInfo* pCreateInfo);
        void DestroyImage(VkImage* pImage);

        VkImageView CreateImageView(VkImageViewCreateInfo* pCreateInfo);
        void DestroyImageView(VkImageView* pImageView);

        VkPipeline CreateGraphicsPipeline(VkGraphicsPipelineCreateInfo* pCreateInfo, VkPipelineCache cache = VK_NULL_HANDLE);
        VkPipeline CreateComputePipeline(VkComputePipelineCreateInfo* pCreateInfo, VkPipelineCache cache = VK_NULL_HANDLE);
        void DestroyPipeline(VkPipeline* pPipeline);

        VkShaderModule CreateShaderModule(VkShaderModuleCreateInfo* pCreateInfo);
        void DestroyShaderModule(VkShaderModule* pShaderModule);

        VkPipelineCache CreatePipelineCache(VkPipelineCacheCreateInfo* pCreateInfo);
        void DestroyPipelineCache(VkPipelineCache* pPipelineCache);

        VkSampler CreateSampler(VkSamplerCreateInfo* pCreateInfo);
        void DestroySampler(VkSampler* pSampler);

        VkFramebuffer CreateFramebuffer(VkFramebufferCreateInfo* pCreateInfo);
        void DestroyFramebuffer(VkFramebuffer* pFramebuffer);

        VkPipelineLayout CreatePipelineLayout(VkPipelineLayoutCreateInfo* pCreateInfo);
        void DestroyPipelineLayout(VkPipelineLayout* pPipelineLayout);

        VkRenderPass CreateRenderPass(VkRenderPassCreateInfo* pCreateInfo);
        void DestroyRenderPass(VkRenderPass* pRenderPass);

        VkSemaphore CreateSemaphore(VkSemaphoreCreateInfo* pCreateInfo);
        void DestroySemaphore(VkSemaphore* pSemaphore);

        VkFence CreateFence(VkFenceCreateInfo* pCreateInfo);
        void DestroyFence(VkFence* pFence);

        VkEvent CreateEvent(VkEventCreateInfo* pCreateInfo);
        void DestroyEvent(VkEvent* pEvent);

        VkQueryPool CreateQueryPool(VkQueryPoolCreateInfo* pCreateInfo);
        void DestroyQueryPool(VkQueryPool* pQueryPool);

        VkDescriptorPool CreateDescriptorPool(VkDescriptorPoolCreateInfo* pCreateInfo);
        void DestroyDescriptorPool(VkDescriptorPool* pPool);

        void BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset);
        void BindBufferMemory2(uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos);
        void BindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset);
        void BindImageMemory2(uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos);
        void FreeMemory(VkDeviceMemory memory);

        void WaitIdle();

        // VK_KHR_swapchain
        VkResult AcquireNextImageKHR(VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pIndex);
        VkResult AcquireNextImage2KHR(VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pIndex);
        VkSwapchainKHR CreateSwapchainKHR(VkSwapchainCreateInfoKHR* pCreateInfo);
        void DestroySwapchainKHR(VkSwapchainKHR* pSwapchain);

    private:
        std::deque<std::function<void()>> deletionQueue;
    };
}
