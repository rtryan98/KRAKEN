#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/GraphicsDevice.h"
#include "Yggdrasil/RenderEngine/Screen.h"
#include "Yggdrasil/RenderEngine/RenderUtil.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"

#include <vector>
#include <iomanip>

namespace Ygg
{
    bool GraphicsDevice::GPU::GetSurfaceSupportKHR(uint32_t queueFamilyIndex, VkSurfaceKHR surface)
    {
        VkBool32 result{};
        vkGetPhysicalDeviceSurfaceSupportKHR(this->handle, queueFamilyIndex, surface, &result);
        return result;
    }

    void GraphicsDevice::GPU::GetSurfacePresentModesKHR(VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes)
    {
        vkGetPhysicalDeviceSurfacePresentModesKHR(this->handle, surface, pPresentModeCount, pPresentModes);
    }

    void GraphicsDevice::GPU::GetSurfaceFormatsKHR(VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats)
    {
        vkGetPhysicalDeviceSurfaceFormatsKHR(this->handle, surface, pSurfaceFormatCount, pSurfaceFormats);
    }

    VkSurfaceCapabilitiesKHR GraphicsDevice::GPU::GetSurfaceCapabilitiesKHR(VkSurfaceKHR surface)
    {
        VkSurfaceCapabilitiesKHR result{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->handle, surface, &result);
        return result;
    }

    inline bool QueueFamilySupportsGraphics(const VkQueueFamilyProperties& properties)
    {
        return properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
    }

    inline bool QueueFamilySupportsCompute(const VkQueueFamilyProperties& properties)
    {
        return properties.queueFlags & VK_QUEUE_COMPUTE_BIT;
    }

    inline bool QueueFamilySupportsTransfer(const VkQueueFamilyProperties& properties)
    {
        return properties.queueFlags & VK_QUEUE_TRANSFER_BIT;
    }

    enum PCI
    {
        AMD = 0x1002,
        ImgTec = 0x1010,
        NVIDIA = 0x10DE,
        ARM = 0x13B5,
        Qualcomm = 0x5143,
        Intel = 0x8086
    };

    std::string PciToString(PCI id)
    {
        switch (id)
        {
        case AMD: return "AMD";
        case ImgTec: return "ImgTec";
        case NVIDIA: return "NVIDIA";
        case ARM: return "ARM";
        case Qualcomm: return "Qualcomm";
        case Intel: return "Intel";
        default:     return "Unknown Vendor.";
        }
    }

    void PrintGpuMemoryInfo(GraphicsDevice* pDevice)
    {
        std::string seperator{ "------|------|--------------|--------------|---------------|-------------|------------------|-----------|--------------" };
        std::stringstream table{};
        table << "Memory Information.\n";
        table << " Heap | Type | DEVICE_LOCAL | HOST_VISIBLE | HOST_COHERENT | HOST_CACHED | LAZILY_ALLOCATED | PROTECTED | SIZE ";
        for (uint32_t i{ 0 }; i < pDevice->gpu.memoryProperties.memoryHeapCount; i++)
        {
            table << "\n" << seperator;
            auto& heap{ pDevice->gpu.memoryProperties.memoryHeaps[i] };
            for (uint32_t j{ 0 }; j < pDevice->gpu.memoryProperties.memoryTypeCount; j++)
            {
                auto& memoryType{ pDevice->gpu.memoryProperties.memoryTypes[j] };
                if (memoryType.heapIndex != i)
                {
                    continue;
                }
                table << "\n";
                table << " " << i << "    ";
                table << "| " << j << "    ";
                if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                {
                    table << "| true         ";
                }
                else
                {
                    table << "| false        ";
                }
                if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
                {
                    table << "| true         ";
                }
                else
                {
                    table << "| false        ";
                }
                if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
                {
                    table << "| true          ";
                }
                else
                {
                    table << "| false         ";
                }
                if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
                {
                    table << "| true        ";
                }
                else
                {
                    table << "| false       ";
                }
                if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
                {
                    table << "| true             ";
                }
                else
                {
                    table << "| false            ";
                }
                if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT)
                {
                    table << "| true      ";
                }
                else
                {
                    table << "| false     ";
                }
                table << "| " << std::fixed << std::setprecision(3) << heap.size * 1.0e-9 << " GB";
            }
        }
        YGG_INFO(table.str());
    }

    void SelectPhysicalDevice(GraphicsDevice* pDevice, GraphicsContext* pContext)
    {
        uint32_t deviceCount{ 0 };
        VkCheck(vkEnumeratePhysicalDevices(pContext->instance, &deviceCount, nullptr));
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        VkCheck(vkEnumeratePhysicalDevices(pContext->instance, &deviceCount, physicalDevices.data()));

        VkPhysicalDevice fallback{ physicalDevices[0] };
        VkPhysicalDevice selected{ fallback };
        for (uint32_t i{ 0 }; i < physicalDevices.size(); i++)
        {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                selected = physicalDevices[i];
            }
        }
        pDevice->gpu.handle = selected;
        vkGetPhysicalDeviceMemoryProperties(pDevice->gpu.handle, &pDevice->gpu.memoryProperties);
        vkGetPhysicalDeviceProperties(pDevice->gpu.handle, &pDevice->gpu.vulkan10Properties);
        VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
        props.pNext = &pDevice->gpu.vulkan11Properties;
        pDevice->gpu.vulkan11Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
        pDevice->gpu.vulkan11Properties.pNext = &pDevice->gpu.vulkan12Properties;
        pDevice->gpu.vulkan12Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
        vkGetPhysicalDeviceProperties2(pDevice->gpu.handle, &props);
        PrintGpuMemoryInfo(pDevice);
    }

    void GraphicsDevice::Create(GraphicsContext* pContext, bool enableAllAvailableFeatures,
        VkPhysicalDeviceFeatures* pRequestedVulkan10Features,
        VkPhysicalDeviceVulkan11Features* pRequestedVulkan11Features,
        VkPhysicalDeviceVulkan12Features* pRequestedVulkan12Features)
    {
        SelectPhysicalDevice(this, pContext);

        uint32_t queueFamilyCount{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties(this->gpu.handle, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(this->gpu.handle, &queueFamilyCount, queueFamilyProperties.data());
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

        VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        queueCreateInfo.queueCount = 1;
        float_t prio{ 1.0f };
        queueCreateInfo.pQueuePriorities = &prio;

        bool mainQueueFound{ false };
        bool asyncComputeQueueFound{ false };
        bool copyQueueFound{ false };

        for (uint32_t i{ 0 }; i < queueFamilyProperties.size(); i++)
        {
            queueCreateInfo.queueFamilyIndex = i;
            if (QueueFamilySupportsGraphics(queueFamilyProperties[i]) &&
                QueueFamilySupportsCompute(queueFamilyProperties[i]) &&
                QueueFamilySupportsTransfer(queueFamilyProperties[i]) &&
                !mainQueueFound)
            {
                mainQueueFound = true;
                this->queues.mainQueueFamilyIndex = i;
                queueCreateInfos.push_back(queueCreateInfo);
            }
            else if (QueueFamilySupportsCompute(queueFamilyProperties[i]) &&
                QueueFamilySupportsTransfer(queueFamilyProperties[i]) &&
                !QueueFamilySupportsGraphics(queueFamilyProperties[i]) &&
                !asyncComputeQueueFound)
            {
                asyncComputeQueueFound = true;
                this->queues.asyncComputeQueueFamilyIndex = i;
                queueCreateInfos.push_back(queueCreateInfo);
            }
            else if (QueueFamilySupportsTransfer(queueFamilyProperties[i]) &&
                !QueueFamilySupportsCompute(queueFamilyProperties[i]) &&
                !QueueFamilySupportsGraphics(queueFamilyProperties[i]) &&
                !copyQueueFound)
            {
                copyQueueFound = true;
                this->queues.copyQueueFamilyIndex = i;
                queueCreateInfos.push_back(queueCreateInfo);
            }
        }

        VkPhysicalDeviceFeatures2 availableVulkan10Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        VkPhysicalDeviceVulkan11Features availableVulkan11Features{};
        VkPhysicalDeviceVulkan12Features availableVulkan12Features{};

        vkGetPhysicalDeviceFeatures2(this->gpu.handle, &availableVulkan10Features);

        this->enabledVulkan10Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        this->enabledVulkan10Features.pNext = &this->enabledVulkan11Features;
        this->enabledVulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        this->enabledVulkan11Features.pNext = &this->enabledVulkan12Features;
        this->enabledVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        this->enabledVulkan12Features.pNext = nullptr;

        if (enableAllAvailableFeatures)
        {
            YGG_INFO("Enabling all available Vulkan features. This may impact performance.");
            this->enabledVulkan10Features = availableVulkan10Features;
            this->enabledVulkan11Features = availableVulkan11Features;
            this->enabledVulkan12Features = availableVulkan12Features;
        }
        else
        {
            if (pRequestedVulkan10Features != nullptr)
            {
                VkBool32* requested{ &(pRequestedVulkan10Features->robustBufferAccess) };
                VkBool32* available{ &(availableVulkan10Features.features.robustBufferAccess) };
                for (uint32_t i{ 0 }; i < (sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32)); i++)
                {
                    if (requested[i] && available[i])
                    {
                        VkBool32* feature{ (&this->enabledVulkan10Features.features.robustBufferAccess) + i };
                        *feature = VK_TRUE;
                    }
                }
            }
            if (pRequestedVulkan11Features != nullptr)
            {
                VkBool32* requested{ &(pRequestedVulkan11Features->storageBuffer16BitAccess) };
                VkBool32* available{ &(availableVulkan11Features.storageBuffer16BitAccess) };
                for (uint32_t i{ 0 }; i < ((sizeof(VkPhysicalDeviceVulkan11Features) - sizeof(void*) - sizeof(VkStructureType)) / sizeof(VkBool32)); i++)
                {
                    if (requested[i] && available[i])
                    {
                        VkBool32* feature{ (&this->enabledVulkan11Features.storageBuffer16BitAccess) + i };
                        *feature = VK_TRUE;
                    }
                }
            }
            if (pRequestedVulkan12Features != nullptr)
            {
                VkBool32* requested{ &(pRequestedVulkan12Features->samplerMirrorClampToEdge) };
                VkBool32* available{ &(availableVulkan12Features.samplerMirrorClampToEdge) };
                for (uint32_t i{ 0 }; i < ((sizeof(VkPhysicalDeviceVulkan12Features) - sizeof(void*) - sizeof(VkStructureType)) / sizeof(VkBool32)); i++)
                {
                    if (requested[i] && available[i])
                    {
                        VkBool32* feature{ (&this->enabledVulkan12Features.samplerMirrorClampToEdge) + i };
                        *feature = VK_TRUE;
                    }
                }
            }
        }

        std::vector<const char*> enabledDeviceExtensions{};
        enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();
        deviceCreateInfo.pEnabledFeatures = nullptr;
        deviceCreateInfo.pNext = &this->enabledVulkan10Features;

        VkCheck(vkCreateDevice(this->gpu.handle, &deviceCreateInfo, nullptr, &this->handle));
        YGG_ASSERT(this->handle);

        if (mainQueueFound)
        {
            vkGetDeviceQueue(this->handle, this->queues.mainQueueFamilyIndex, 0, &this->queues.mainQueue);
        }
        else
        {
            YGG_CRITICAL("No main queue family found.");
        }
        if (asyncComputeQueueFound)
        {
            vkGetDeviceQueue(this->handle, this->queues.asyncComputeQueueFamilyIndex, 0, &this->queues.asyncComputeQueue);
        }
        else
        {
            YGG_INFO("No async compute queue family found. Mapping to main queue.");
            vkGetDeviceQueue(this->handle, this->queues.mainQueueFamilyIndex, 0, &this->queues.asyncComputeQueue);
            this->queues.asyncComputeQueueFamilyIndex = this->queues.mainQueueFamilyIndex;
        }
        if (copyQueueFound)
        {
            vkGetDeviceQueue(this->handle, this->queues.copyQueueFamilyIndex, 0, &this->queues.copyQueue);
        }
        else
        {
            YGG_INFO("No copy queue family found. Mapping to main queue.");
            vkGetDeviceQueue(this->handle, this->queues.mainQueueFamilyIndex, 0, &this->queues.copyQueue);
            this->queues.copyQueueFamilyIndex = this->queues.mainQueueFamilyIndex;
        }
    }

    void GraphicsDevice::Destroy()
    {
        WaitIdle();
        for (auto it{ this->deletionQueue.rbegin() }; it != this->deletionQueue.rend(); it++)
        {
            (*it)();
        }
        vkDestroyDevice(this->handle, nullptr);
    }

    void GraphicsDevice::PushObjectDeletion(const std::function<void()>&& mFunction)
    {
        YGG_TRACE("Called");
        this->deletionQueue.push_back(mFunction);
    }

    VkCommandPool GraphicsDevice::CreateCommandPool(VkCommandPoolCreateInfo* pCreateInfo)
    {
        VkCommandPool result{};
        VkCheck(vkCreateCommandPool(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyCommandPool(VkCommandPool* pPool)
    {
        DestroyVkObject(pPool, vkDestroyCommandPool, this->handle);
    }

    VkBuffer GraphicsDevice::CreateBuffer(VkBufferCreateInfo* pCreateInfo)
    {
        VkBuffer result{};
        VkCheck(vkCreateBuffer(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyBuffer(VkBuffer* pBuffer)
    {
        DestroyVkObject(pBuffer, vkDestroyBuffer, this->handle);
    }

    VkBufferView GraphicsDevice::CreateBufferView(VkBufferViewCreateInfo* pCreateInfo)
    {
        VkBufferView result{};
        VkCheck(vkCreateBufferView(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyBufferView(VkBufferView* pBufferView)
    {
        DestroyVkObject(pBufferView, vkDestroyBufferView, this->handle);
    }

    VkImage GraphicsDevice::CreateImage(VkImageCreateInfo* pCreateInfo)
    {
        VkImage result{};
        VkCheck(vkCreateImage(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyImage(VkImage* pImage)
    {
        DestroyVkObject(pImage, vkDestroyImage, this->handle);
    }

    VkImageView GraphicsDevice::CreateImageView(VkImageViewCreateInfo* pCreateInfo)
    {
        VkImageView result{};
        VkCheck(vkCreateImageView(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyImageView(VkImageView* pImageView)
    {
        DestroyVkObject(pImageView, vkDestroyImageView, this->handle);
    }

    VkPipeline GraphicsDevice::CreateGraphicsPipeline(VkGraphicsPipelineCreateInfo* pCreateInfo, VkPipelineCache cache)
    {
        VkPipeline result{};
        VkCheck(vkCreateGraphicsPipelines(this->handle, cache, 1, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyPipeline(VkPipeline* pPipeline)
    {
        DestroyVkObject(pPipeline, vkDestroyPipeline, this->handle);
    }

    VkPipeline GraphicsDevice::CreateComputePipeline(VkComputePipelineCreateInfo* pCreateInfo, VkPipelineCache cache)
    {
        VkPipeline result{};
        VkCheck(vkCreateComputePipelines(this->handle, cache, 1, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    VkShaderModule GraphicsDevice::CreateShaderModule(VkShaderModuleCreateInfo* pCreateInfo)
    {
        VkShaderModule result{};
        VkCheck(vkCreateShaderModule(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyShaderModule(VkShaderModule* pShaderModule)
    {
        DestroyVkObject(pShaderModule, vkDestroyShaderModule, this->handle);
    }

    VkPipelineCache GraphicsDevice::CreatePipelineCache(VkPipelineCacheCreateInfo* pCreateInfo)
    {
        VkPipelineCache result{};
        VkCheck(vkCreatePipelineCache(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyPipelineCache(VkPipelineCache* pPipelineCache)
    {
        DestroyVkObject(pPipelineCache, vkDestroyPipelineCache, this->handle);
    }

    VkSampler GraphicsDevice::CreateSampler(VkSamplerCreateInfo* pCreateInfo)
    {
        VkSampler result{};
        VkCheck(vkCreateSampler(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroySampler(VkSampler* pSampler)
    {
        DestroyVkObject(pSampler, vkDestroySampler, this->handle);
    }

    VkFramebuffer GraphicsDevice::CreateFramebuffer(VkFramebufferCreateInfo* pCreateInfo)
    {
        VkFramebuffer result{};
        VkCheck(vkCreateFramebuffer(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyFramebuffer(VkFramebuffer* pFramebuffer)
    {
        DestroyVkObject(pFramebuffer, vkDestroyFramebuffer, this->handle);
    }

    VkPipelineLayout GraphicsDevice::CreatePipelineLayout(VkPipelineLayoutCreateInfo* pCreateInfo)
    {
        VkPipelineLayout result{};
        VkCheck(vkCreatePipelineLayout(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyPipelineLayout(VkPipelineLayout* pPipelineLayout)
    {
        DestroyVkObject(pPipelineLayout, vkDestroyPipelineLayout, this->handle);
    }

    VkRenderPass GraphicsDevice::CreateRenderPass(VkRenderPassCreateInfo* pCreateInfo)
    {
        VkRenderPass result{};
        VkCheck(vkCreateRenderPass(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyRenderPass(VkRenderPass* pRenderPass)
    {
        DestroyVkObject(pRenderPass, vkDestroyRenderPass, this->handle);
    }

    VkSemaphore GraphicsDevice::CreateSemaphore(VkSemaphoreCreateInfo* pCreateInfo)
    {
        VkSemaphore result{};
        VkCheck(vkCreateSemaphore(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroySemaphore(VkSemaphore* pSemaphore)
    {
        DestroyVkObject(pSemaphore, vkDestroySemaphore, this->handle);
    }

    VkFence GraphicsDevice::CreateFence(VkFenceCreateInfo* pCreateInfo)
    {
        VkFence result{};
        VkCheck(vkCreateFence(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyFence(VkFence* pFence)
    {
        DestroyVkObject(pFence, vkDestroyFence, this->handle);
    }

    VkEvent GraphicsDevice::CreateEvent(VkEventCreateInfo* pCreateInfo)
    {
        VkEvent result{};
        VkCheck(vkCreateEvent(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyEvent(VkEvent* pEvent)
    {
        DestroyVkObject(pEvent, vkDestroyEvent, this->handle);
    }

    VkQueryPool GraphicsDevice::CreateQueryPool(VkQueryPoolCreateInfo* pCreateInfo)
    {
        VkQueryPool result{};
        VkCheck(vkCreateQueryPool(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyQueryPool(VkQueryPool* pQueryPool)
    {
        DestroyVkObject(pQueryPool, vkDestroyQueryPool, this->handle);
    }

    VkDescriptorPool GraphicsDevice::CreateDescriptorPool(VkDescriptorPoolCreateInfo* pCreateInfo)
    {
        VkDescriptorPool result{};
        VkCheck(vkCreateDescriptorPool(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroyDescriptorPool(VkDescriptorPool* pPool)
    {
        DestroyVkObject(pPool, vkDestroyDescriptorPool, this->handle);
    }

    void GraphicsDevice::BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset)
    {
        VkCheck(vkBindBufferMemory(this->handle, buffer, memory, memoryOffset));
    }

    void GraphicsDevice::BindBufferMemory2(uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos)
    {
        VkCheck(vkBindBufferMemory2(this->handle, bindInfoCount, pBindInfos));
    }

    void GraphicsDevice::BindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset)
    {
        VkCheck(vkBindImageMemory(this->handle, image, memory, memoryOffset));
    }

    void GraphicsDevice::BindImageMemory2(uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos)
    {
        VkCheck(vkBindImageMemory2(this->handle, bindInfoCount, pBindInfos));
    }

    void GraphicsDevice::FreeMemory(VkDeviceMemory memory)
    {
        vkFreeMemory(this->handle, memory, nullptr);
    }

    VkResult GraphicsDevice::AcquireNextImageKHR(VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pIndex)
    {
        return vkAcquireNextImageKHR(this->handle, swapchain, timeout, semaphore, fence, pIndex);
    }

    VkResult GraphicsDevice::AcquireNextImage2KHR(VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pIndex)
    {
        return vkAcquireNextImage2KHR(this->handle, pAcquireInfo, pIndex);
    }

    VkSwapchainKHR GraphicsDevice::CreateSwapchainKHR(VkSwapchainCreateInfoKHR* pCreateInfo)
    {
        VkSwapchainKHR result{};
        VkCheck(vkCreateSwapchainKHR(this->handle, pCreateInfo, nullptr, &result));
        YGG_ASSERT(result);
        return result;
    }

    void GraphicsDevice::DestroySwapchainKHR(VkSwapchainKHR* pSwapchain)
    {
        DestroyVkObject(pSwapchain, vkDestroySwapchainKHR, this->handle);
    }

    void GraphicsDevice::WaitIdle()
    {
        vkDeviceWaitIdle(this->handle);
    }
}
