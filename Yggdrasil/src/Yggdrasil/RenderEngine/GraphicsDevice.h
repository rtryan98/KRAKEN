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
            VkPhysicalDeviceFeatures* pRequestedFeatures = nullptr,
            VkPhysicalDeviceVulkan11Features* pRequestedVulkan11Features = nullptr,
            VkPhysicalDeviceVulkan12Features* pRequestedVulkan12Features = nullptr);
        void Destroy();

        void PushObjectDeletion(const std::function<void()>&& mFunction);

        VkCommandPool CreateCommandPool(VkCommandPoolCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyCommandPool(VkCommandPool* pPool);

        VkBuffer CreateBuffer(VkBufferCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyBuffer(VkBuffer* pBuffer);

        VkBufferView CreateBufferView(VkBufferViewCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyBufferView(VkBufferView* pBufferView);

        VkImage CreateImage(VkImageCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyImage(VkImage* pImage);

        VkImageView CreateImageView(VkImageViewCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyImageView(VkImageView* pImageView);

        VkPipeline CreateGraphicsPipeline(VkGraphicsPipelineCreateInfo* pCreateInfo, VkPipelineCache cache = VK_NULL_HANDLE, const char* name = nullptr);
        VkPipeline CreateComputePipeline(VkComputePipelineCreateInfo* pCreateInfo, VkPipelineCache cache = VK_NULL_HANDLE, const char* name = nullptr);
        void DestroyPipeline(VkPipeline* pPipeline);

        VkShaderModule CreateShaderModule(VkShaderModuleCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyShaderModule(VkShaderModule* pShaderModule);

        VkPipelineCache CreatePipelineCache(VkPipelineCacheCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyPipelineCache(VkPipelineCache* pPipelineCache);

        VkSampler CreateSampler(VkSamplerCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroySampler(VkSampler* pSampler);

        VkFramebuffer CreateFramebuffer(VkFramebufferCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyFramebuffer(VkFramebuffer* pFramebuffer);

        VkPipelineLayout CreatePipelineLayout(VkPipelineLayoutCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyPipelineLayout(VkPipelineLayout* pPipelineLayout);

        VkRenderPass CreateRenderPass(VkRenderPassCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyRenderPass(VkRenderPass* pRenderPass);

        VkSemaphore CreateSemaphore(VkSemaphoreCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroySemaphore(VkSemaphore* pSemaphore);

        VkFence CreateFence(VkFenceCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyFence(VkFence* pFence);

        VkEvent CreateEvent(VkEventCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyEvent(VkEvent* pEvent);

        VkQueryPool CreateQueryPool(VkQueryPoolCreateInfo* pCreateInfo, const char* name = nullptr);
        void DestroyQueryPool(VkQueryPool* pQueryPool);

        VkDescriptorPool CreateDescriptorPool(VkDescriptorPoolCreateInfo* pCreateInfo, const char* name = nullptr);
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
        VkSwapchainKHR CreateSwapchainKHR(VkSwapchainCreateInfoKHR* pCreateInfo, const char* name = nullptr);
        void DestroySwapchainKHR(VkSwapchainKHR* pSwapchain);

    private:
        std::deque<std::function<void()>> deletionQueue;
    };
}
