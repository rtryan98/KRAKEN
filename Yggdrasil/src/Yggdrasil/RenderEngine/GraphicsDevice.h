#pragma once

#include <vulkan/vulkan.h>
#include <deque>
#include <functional>

namespace Ygg
{
    class CGraphicsContext;

    class CGraphicsDevice
    {
    private:
        class CGPU;
        struct SFeatures;
        struct SQueues;

    public:
        void Create(CGraphicsContext* pContext,
            VkPhysicalDeviceFeatures* pRequestedFeatures = nullptr,
            VkPhysicalDeviceVulkan11Features* pRequestedVulkan11Features = nullptr,
            VkPhysicalDeviceVulkan12Features* pRequestedVulkan12Features = nullptr);
        void Destroy();
        const CGPU& GetGPU() const;
        const SFeatures& GetFeatures() const;
        const SQueues& GetQueues() const;
        VkDevice GetHandle();

        void PushObjectDeletion(std::function<void()>&& mFunction);

        VkCommandPool CreateCommandPool(VkCommandPoolCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyCommandPool(VkCommandPool* pPool);

        VkBuffer CreateBuffer(VkBufferCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyBuffer(VkBuffer* pBuffer);

        VkBufferView CreateBufferView(VkBufferViewCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyBufferView(VkBufferView* pBufferView);

        VkImage CreateImage(VkImageCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyImage(VkImage* pImage);

        VkImageView CreateImageView(VkImageViewCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyImageView(VkImageView* pImageView);

        VkPipeline CreateGraphicsPipeline(VkGraphicsPipelineCreateInfo* pCreateInfo, VkPipelineCache cache = VK_NULL_HANDLE, const char* name = nullptr) const;
        VkPipeline CreateComputePipeline(VkComputePipelineCreateInfo* pCreateInfo, VkPipelineCache cache = VK_NULL_HANDLE, const char* name = nullptr) const;
        void DestroyPipeline(VkPipeline* pPipeline);

        VkShaderModule CreateShaderModule(VkShaderModuleCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyShaderModule(VkShaderModule* pShaderModule);

        VkPipelineCache CreatePipelineCache(VkPipelineCacheCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyPipelineCache(VkPipelineCache* pPipelineCache);

        VkSampler CreateSampler(VkSamplerCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroySampler(VkSampler* pSampler);

        VkFramebuffer CreateFramebuffer(VkFramebufferCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyFramebuffer(VkFramebuffer* pFramebuffer);

        VkPipelineLayout CreatePipelineLayout(VkPipelineLayoutCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyPipelineLayout(VkPipelineLayout* pPipelineLayout);

        VkRenderPass CreateRenderPass(VkRenderPassCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyRenderPass(VkRenderPass* pRenderPass);

        VkSemaphore CreateSemaphore(VkSemaphoreCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroySemaphore(VkSemaphore* pSemaphore);

        VkFence CreateFence(VkFenceCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyFence(VkFence* pFence);

        VkEvent CreateEvent(VkEventCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyEvent(VkEvent* pEvent);

        VkQueryPool CreateQueryPool(VkQueryPoolCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyQueryPool(VkQueryPool* pQueryPool);

        VkDescriptorPool CreateDescriptorPool(VkDescriptorPoolCreateInfo* pCreateInfo, const char* name = nullptr) const;
        void DestroyDescriptorPool(VkDescriptorPool* pPool);

        VkSwapchainKHR CreateSwapchainKHR(VkSwapchainCreateInfoKHR* pCreateInfo, const char* name = nullptr) const;
        void DestroySwapchainKHR(VkSwapchainKHR* pSwapchain);

        void BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset);
        void BindBufferMemory2(uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos);
        void BindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset);
        void BindImageMemory2(uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos);
        void FreeMemory(VkDeviceMemory memory);

        void WaitIdle() const;

        VkCommandBuffer AllocateCommandBuffer(VkCommandPool pool, VkCommandBufferLevel level, const char* name = nullptr) const;
        void ResetCommandPool(VkCommandPool pool, VkCommandPoolResetFlags flags);
        void ResetFence(VkFence fence);

        VkResult AcquireNextImageKHR(VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pIndex);
        VkResult AcquireNextImage2KHR(VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pIndex);

    private:
        VkDevice m_handle;

        struct SFeatures
        {
            VkPhysicalDeviceFeatures2 enabledVulkan10Features{};
            VkPhysicalDeviceVulkan11Features enabledVulkan11Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
            VkPhysicalDeviceVulkan12Features enabledVulkan12Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        } m_features;

        struct SQueues
        {
            uint32_t mainQueueFamilyIndex;
            VkQueue mainQueue;

            uint32_t asyncComputeQueueFamilyIndex;
            VkQueue asyncComputeQueue;

            VkQueue copyQueue;
            uint32_t copyQueueFamilyIndex;
        } m_queues;

        class CGPU
        {
        private:
            struct SData;
        public:
            SData& GetData();
            void SelectPhysicalDevice(CGraphicsContext* pContext);

            // VK_KHR_surface
            bool GetSurfaceSupportKHR(uint32_t queueFamilyIndex, VkSurfaceKHR surface) const;
            void GetSurfacePresentModesKHR(VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes) const;
            void GetSurfaceFormatsKHR(VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats) const;
            VkSurfaceCapabilitiesKHR GetSurfaceCapabilitiesKHR(VkSurfaceKHR surface) const;
        private:
            struct SData
            {
                VkPhysicalDevice handle;
                VkPhysicalDeviceProperties vulkan10Properties;
                VkPhysicalDeviceVulkan11Properties vulkan11Properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES };
                VkPhysicalDeviceVulkan12Properties vulkan12Properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES };
                VkPhysicalDeviceMemoryProperties memoryProperties;
            } m_data;

            void PrintGpuInfo() const;
            void CGraphicsDevice::CGPU::PrintGpuMemoryInfo() const;
        } m_gpu;

        std::deque<std::function<void()>> m_deletionQueue;
    };
}
