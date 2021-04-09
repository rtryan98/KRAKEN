#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/GraphicsDevice.h"
#include "Yggdrasil/RenderEngine/Screen.h"
#include "Yggdrasil/RenderEngine/RenderUtil.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"

#include <vector>
#include <iomanip>

namespace Ygg
{
    CGraphicsDevice::CGPU::SData& CGraphicsDevice::CGPU::GetData()
    {
        return this->m_data;
    }

    bool CGraphicsDevice::CGPU::GetSurfaceSupportKHR(uint32_t queueFamilyIndex, VkSurfaceKHR surface) const
    {
        VkBool32 result{};
        vkGetPhysicalDeviceSurfaceSupportKHR(this->m_data.handle, queueFamilyIndex, surface, &result);
        return result;
    }

    void CGraphicsDevice::CGPU::GetSurfacePresentModesKHR(VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes) const
    {
        vkGetPhysicalDeviceSurfacePresentModesKHR(this->m_data.handle, surface, pPresentModeCount, pPresentModes);
    }

    void CGraphicsDevice::CGPU::GetSurfaceFormatsKHR(VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats) const
    {
        vkGetPhysicalDeviceSurfaceFormatsKHR(this->m_data.handle, surface, pSurfaceFormatCount, pSurfaceFormats);
    }

    VkSurfaceCapabilitiesKHR CGraphicsDevice::CGPU::GetSurfaceCapabilitiesKHR(VkSurfaceKHR surface) const
    {
        VkSurfaceCapabilitiesKHR result{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->m_data.handle, surface, &result);
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

    enum PCI : uint32_t
    {
        AMD = 0x1002,
        ImgTec = 0x1010,
        NVIDIA = 0x10DE,
        ARM = 0x13B5,
        Qualcomm = 0x5143,
        Intel = 0x8086
    };

    std::string PciToString(uint32_t id)
    {
        switch (id)
        {
        case PCI::AMD: return "AMD";
        case PCI::ImgTec: return "ImgTec";
        case PCI::NVIDIA: return "NVIDIA";
        case PCI::ARM: return "ARM";
        case PCI::Qualcomm: return "Qualcomm";
        case PCI::Intel: return "Intel";
        default:     return "Unknown Vendor.";
        }
    }

    void CGraphicsDevice::CGPU::PrintGpuInfo() const
    {
        YGG_INFO("Selected GPU '{0} {1}'",
            PciToString(this->m_data.vulkan10Properties.vendorID),
            this->m_data.vulkan10Properties.deviceName);
    }

    void CGraphicsDevice::CGPU::PrintGpuMemoryInfo() const
    {
        std::string seperator{ "------|------|--------------|--------------|---------------|-------------|------------------|-----------|--------------" };
        std::stringstream table{};
        table << "Vulkan Memory Information.\n";
        table << " Heap | Type | DEVICE_LOCAL | HOST_VISIBLE | HOST_COHERENT | HOST_CACHED | LAZILY_ALLOCATED | PROTECTED | SIZE ";
        for (uint32_t i{ 0 }; i < this->m_data.memoryProperties.memoryHeapCount; i++)
        {
            table << "\n" << seperator;
            const VkMemoryHeap heap{ this->m_data.memoryProperties.memoryHeaps[i] };
            for (uint32_t j{ 0 }; j < this->m_data.memoryProperties.memoryTypeCount; j++)
            {
                const VkMemoryType memoryType{ this->m_data.memoryProperties.memoryTypes[j] };
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

    void CGraphicsDevice::CGPU::SelectPhysicalDevice(CGraphicsContext* pContext)
    {
        uint32_t deviceCount{ 0 };
        RenderUtil::VkCheck(vkEnumeratePhysicalDevices(pContext->GetVkInstance(), &deviceCount, nullptr));
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        RenderUtil::VkCheck(vkEnumeratePhysicalDevices(pContext->GetVkInstance(), &deviceCount, physicalDevices.data()));

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
        this->m_data.handle = selected;
        vkGetPhysicalDeviceMemoryProperties(this->m_data.handle, &this->m_data.memoryProperties);
        vkGetPhysicalDeviceProperties(this->m_data.handle, &this->m_data.vulkan10Properties);
        VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
        props.pNext = &this->m_data.vulkan11Properties;
        this->m_data.vulkan11Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
        this->m_data.vulkan11Properties.pNext = &this->m_data.vulkan12Properties;
        this->m_data.vulkan12Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
        vkGetPhysicalDeviceProperties2(this->m_data.handle, &props);
        PrintGpuInfo();
        PrintGpuMemoryInfo();
    }

    void CGraphicsDevice::Create(CGraphicsContext* pContext,
        VkPhysicalDeviceFeatures* pRequestedVulkan10Features,
        VkPhysicalDeviceVulkan11Features* pRequestedVulkan11Features,
        VkPhysicalDeviceVulkan12Features* pRequestedVulkan12Features)
    {
        this->m_gpu.SelectPhysicalDevice(pContext);

        uint32_t queueFamilyCount{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties(this->m_gpu.GetData().handle, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(this->m_gpu.GetData().handle, &queueFamilyCount, queueFamilyProperties.data());
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
                this->m_queues.mainQueueFamilyIndex = i;
                queueCreateInfos.push_back(queueCreateInfo);
            }
            else if (QueueFamilySupportsCompute(queueFamilyProperties[i]) &&
                QueueFamilySupportsTransfer(queueFamilyProperties[i]) &&
                !QueueFamilySupportsGraphics(queueFamilyProperties[i]) &&
                !asyncComputeQueueFound)
            {
                asyncComputeQueueFound = true;
                this->m_queues.asyncComputeQueueFamilyIndex = i;
                queueCreateInfos.push_back(queueCreateInfo);
            }
            else if (QueueFamilySupportsTransfer(queueFamilyProperties[i]) &&
                !QueueFamilySupportsCompute(queueFamilyProperties[i]) &&
                !QueueFamilySupportsGraphics(queueFamilyProperties[i]) &&
                !copyQueueFound)
            {
                copyQueueFound = true;
                this->m_queues.copyQueueFamilyIndex = i;
                queueCreateInfos.push_back(queueCreateInfo);
            }
        }

        VkPhysicalDeviceFeatures2 availableVulkan10Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        VkPhysicalDeviceVulkan11Features availableVulkan11Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
        VkPhysicalDeviceVulkan12Features availableVulkan12Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };

        availableVulkan10Features.pNext = &availableVulkan11Features;
        availableVulkan11Features.pNext = &availableVulkan12Features;

        vkGetPhysicalDeviceFeatures2(this->m_gpu.GetData().handle, &availableVulkan10Features);

        this->m_features.enabledVulkan10Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        this->m_features.enabledVulkan10Features.pNext = &this->m_features.enabledVulkan11Features;
        this->m_features.enabledVulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        this->m_features.enabledVulkan11Features.pNext = &this->m_features.enabledVulkan12Features;
        this->m_features.enabledVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        this->m_features.enabledVulkan12Features.pNext = nullptr;

        YGG_TRACE("Checking for enabled Vulkan Core Features...");
        if (pRequestedVulkan10Features != nullptr)
        {
            VkBool32* requested{ &(pRequestedVulkan10Features->robustBufferAccess) };
            VkBool32* available{ &(availableVulkan10Features.features.robustBufferAccess) };
            for (uint32_t i{ 0 }; i < (sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32)); i++)
            {
                if (requested[i] && available[i])
                {
                    VkBool32* feature{ (&this->m_features.enabledVulkan10Features.features.robustBufferAccess) + i };
                    *feature = VK_TRUE;
                    YGG_INFO("Requested Vulkan 1.0 Feature '{0}' is ACTIVE.", RenderUtil::VkDeviceFeatures10ToString(i));
                }
                else if(requested[i])
                {
                    YGG_INFO("Requested Vulkan 1.0 Feature '{0}' is NOT AVAILABLE.", RenderUtil::VkDeviceFeatures10ToString(i));
                }
            }
        }
        if (pRequestedVulkan11Features != nullptr)
        {
            VkBool32* requested{ &(pRequestedVulkan11Features->storageBuffer16BitAccess) };
            VkBool32* available{ &(availableVulkan11Features.storageBuffer16BitAccess) };
            for (uint32_t i{ 0 }; i < ((sizeof(VkPhysicalDeviceVulkan11Features) - sizeof(void*) - sizeof(VkStructureType)) / sizeof(VkBool32)) - 1; i++)
            {
                if (requested[i] && available[i])
                {
                    VkBool32* feature{ (&this->m_features.enabledVulkan11Features.storageBuffer16BitAccess) + i };
                    *feature = VK_TRUE;
                    YGG_INFO("Requested Vulkan 1.1 Feature '{0}' is ACTIVE.", RenderUtil::VkDeviceFeatures11ToString(i));
                }
                else if (requested[i])
                {
                    YGG_INFO("Requested Vulkan 1.1 Feature '{0}' is NOT AVAILABLE.", RenderUtil::VkDeviceFeatures11ToString(i));
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
                    VkBool32* feature{ (&this->m_features.enabledVulkan12Features.samplerMirrorClampToEdge) + i };
                    *feature = VK_TRUE;
                    YGG_INFO("Requested Vulkan 1.2 Feature '{0}' is ACTIVE.", RenderUtil::VkDeviceFeatures12ToString(i));
                }
                else if (requested[i])
                {
                    YGG_WARN("Requested Vulkan 1.2 Feature '{0}' is NOT AVAILABLE.", RenderUtil::VkDeviceFeatures12ToString(i));
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
        deviceCreateInfo.pNext = &this->m_features.enabledVulkan10Features;

        RenderUtil::VkCheck(vkCreateDevice(this->m_gpu.GetData().handle, &deviceCreateInfo, nullptr, &this->m_handle));
        YGG_ASSERT(this->m_handle);

        if (mainQueueFound)
        {
            vkGetDeviceQueue(this->m_handle, this->m_queues.mainQueueFamilyIndex, 0, &this->m_queues.mainQueue);
        }
        else
        {
            YGG_CRITICAL("No main queue family found.");
        }
        if (asyncComputeQueueFound)
        {
            vkGetDeviceQueue(this->m_handle, this->m_queues.asyncComputeQueueFamilyIndex, 0, &this->m_queues.asyncComputeQueue);
        }
        else
        {
            YGG_INFO("No async compute queue family found. Mapping to main queue.");
            vkGetDeviceQueue(this->m_handle, this->m_queues.mainQueueFamilyIndex, 0, &this->m_queues.asyncComputeQueue);
            this->m_queues.asyncComputeQueueFamilyIndex = this->m_queues.mainQueueFamilyIndex;
        }
        if (copyQueueFound)
        {
            vkGetDeviceQueue(this->m_handle, this->m_queues.copyQueueFamilyIndex, 0, &this->m_queues.copyQueue);
        }
        else
        {
            YGG_INFO("No copy queue family found. Mapping to main queue.");
            vkGetDeviceQueue(this->m_handle, this->m_queues.mainQueueFamilyIndex, 0, &this->m_queues.copyQueue);
            this->m_queues.copyQueueFamilyIndex = this->m_queues.mainQueueFamilyIndex;
        }
    }

    void CGraphicsDevice::Destroy()
    {
        WaitIdle();
        for (auto it{ this->m_deletionQueue.rbegin() }; it != this->m_deletionQueue.rend(); it++)
        {
            (*it)();
        }
        vkDestroyDevice(this->m_handle, nullptr);
    }

    const CGraphicsDevice::CGPU& CGraphicsDevice::GetGPU() const
    {
        return this->m_gpu;
    }

    const CGraphicsDevice::SFeatures& CGraphicsDevice::GetFeatures() const
    {
        return this->m_features;
    }

    VkDevice CGraphicsDevice::GetHandle()
    {
        return this->m_handle;
    }

    void CGraphicsDevice::PushObjectDeletion(const std::function<void()>&& mFunction)
    {
        YGG_TRACE("Called");
        this->m_deletionQueue.push_back(mFunction);
    }

    VkCommandPool CGraphicsDevice::CreateCommandPool(VkCommandPoolCreateInfo* pCreateInfo, const char* name) const
    {
        VkCommandPool result{};
        RenderUtil::VkCheck(vkCreateCommandPool(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_COMMAND_POOL, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyCommandPool(VkCommandPool* pPool)
    {
        RenderUtil::DestroyVkObject(pPool, vkDestroyCommandPool, this->m_handle);
    }

    VkBuffer CGraphicsDevice::CreateBuffer(VkBufferCreateInfo* pCreateInfo, const char* name) const
    {
        VkBuffer result{};
        RenderUtil::VkCheck(vkCreateBuffer(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_BUFFER, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyBuffer(VkBuffer* pBuffer)
    {
        RenderUtil::DestroyVkObject(pBuffer, vkDestroyBuffer, this->m_handle);
    }

    VkBufferView CGraphicsDevice::CreateBufferView(VkBufferViewCreateInfo* pCreateInfo, const char* name) const
    {
        VkBufferView result{};
        RenderUtil::VkCheck(vkCreateBufferView(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_BUFFER_VIEW, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyBufferView(VkBufferView* pBufferView)
    {
        RenderUtil::DestroyVkObject(pBufferView, vkDestroyBufferView, this->m_handle);
    }

    VkImage CGraphicsDevice::CreateImage(VkImageCreateInfo* pCreateInfo, const char* name) const
    {
        VkImage result{};
        RenderUtil::VkCheck(vkCreateImage(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_IMAGE, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyImage(VkImage* pImage)
    {
        RenderUtil::DestroyVkObject(pImage, vkDestroyImage, this->m_handle);
    }

    VkImageView CGraphicsDevice::CreateImageView(VkImageViewCreateInfo* pCreateInfo, const char* name) const
    {
        VkImageView result{};
        RenderUtil::VkCheck(vkCreateImageView(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_IMAGE_VIEW, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyImageView(VkImageView* pImageView)
    {
        RenderUtil::DestroyVkObject(pImageView, vkDestroyImageView, this->m_handle);
    }

    VkPipeline CGraphicsDevice::CreateGraphicsPipeline(VkGraphicsPipelineCreateInfo* pCreateInfo, VkPipelineCache cache, const char* name) const
    {
        VkPipeline result{};
        RenderUtil::VkCheck(vkCreateGraphicsPipelines(this->m_handle, cache, 1, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_PIPELINE, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyPipeline(VkPipeline* pPipeline)
    {
        RenderUtil::DestroyVkObject(pPipeline, vkDestroyPipeline, this->m_handle);
    }

    VkPipeline CGraphicsDevice::CreateComputePipeline(VkComputePipelineCreateInfo* pCreateInfo, VkPipelineCache cache, const char* name) const
    {
        VkPipeline result{};
        RenderUtil::VkCheck(vkCreateComputePipelines(this->m_handle, cache, 1, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_PIPELINE, name);
        YGG_ASSERT(result);
        return result;
    }

    VkShaderModule CGraphicsDevice::CreateShaderModule(VkShaderModuleCreateInfo* pCreateInfo, const char* name) const
    {
        VkShaderModule result{};
        RenderUtil::VkCheck(vkCreateShaderModule(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_SHADER_MODULE, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyShaderModule(VkShaderModule* pShaderModule)
    {
        RenderUtil::DestroyVkObject(pShaderModule, vkDestroyShaderModule, this->m_handle);
    }

    VkPipelineCache CGraphicsDevice::CreatePipelineCache(VkPipelineCacheCreateInfo* pCreateInfo, const char* name) const
    {
        VkPipelineCache result{};
        RenderUtil::VkCheck(vkCreatePipelineCache(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_PIPELINE_CACHE, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyPipelineCache(VkPipelineCache* pPipelineCache)
    {
        RenderUtil::DestroyVkObject(pPipelineCache, vkDestroyPipelineCache, this->m_handle);
    }

    VkSampler CGraphicsDevice::CreateSampler(VkSamplerCreateInfo* pCreateInfo, const char* name) const
    {
        VkSampler result{};
        RenderUtil::VkCheck(vkCreateSampler(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_SAMPLER, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroySampler(VkSampler* pSampler)
    {
        RenderUtil::DestroyVkObject(pSampler, vkDestroySampler, this->m_handle);
    }

    VkFramebuffer CGraphicsDevice::CreateFramebuffer(VkFramebufferCreateInfo* pCreateInfo, const char* name) const
    {
        VkFramebuffer result{};
        RenderUtil::VkCheck(vkCreateFramebuffer(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_FRAMEBUFFER, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyFramebuffer(VkFramebuffer* pFramebuffer)
    {
        RenderUtil::DestroyVkObject(pFramebuffer, vkDestroyFramebuffer, this->m_handle);
    }

    VkPipelineLayout CGraphicsDevice::CreatePipelineLayout(VkPipelineLayoutCreateInfo* pCreateInfo, const char* name) const
    {
        VkPipelineLayout result{};
        RenderUtil::VkCheck(vkCreatePipelineLayout(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_PIPELINE_LAYOUT, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyPipelineLayout(VkPipelineLayout* pPipelineLayout)
    {
        RenderUtil::DestroyVkObject(pPipelineLayout, vkDestroyPipelineLayout, this->m_handle);
    }

    VkRenderPass CGraphicsDevice::CreateRenderPass(VkRenderPassCreateInfo* pCreateInfo, const char* name) const
    {
        VkRenderPass result{};
        RenderUtil::VkCheck(vkCreateRenderPass(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_RENDER_PASS, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyRenderPass(VkRenderPass* pRenderPass)
    {
        RenderUtil::DestroyVkObject(pRenderPass, vkDestroyRenderPass, this->m_handle);
    }

    VkSemaphore CGraphicsDevice::CreateSemaphore(VkSemaphoreCreateInfo* pCreateInfo, const char* name) const
    {
        VkSemaphore result{};
        RenderUtil::VkCheck(vkCreateSemaphore(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_SEMAPHORE, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroySemaphore(VkSemaphore* pSemaphore)
    {
        RenderUtil::DestroyVkObject(pSemaphore, vkDestroySemaphore, this->m_handle);
    }

    VkFence CGraphicsDevice::CreateFence(VkFenceCreateInfo* pCreateInfo, const char* name) const
    {
        VkFence result{};
        RenderUtil::VkCheck(vkCreateFence(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_FENCE, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyFence(VkFence* pFence)
    {
        RenderUtil::DestroyVkObject(pFence, vkDestroyFence, this->m_handle);
    }

    VkEvent CGraphicsDevice::CreateEvent(VkEventCreateInfo* pCreateInfo, const char* name) const
    {
        VkEvent result{};
        RenderUtil::VkCheck(vkCreateEvent(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_EVENT, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyEvent(VkEvent* pEvent)
    {
        RenderUtil::DestroyVkObject(pEvent, vkDestroyEvent, this->m_handle);
    }

    VkQueryPool CGraphicsDevice::CreateQueryPool(VkQueryPoolCreateInfo* pCreateInfo, const char* name) const
    {
        VkQueryPool result{};
        RenderUtil::VkCheck(vkCreateQueryPool(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_QUERY_POOL, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyQueryPool(VkQueryPool* pQueryPool)
    {
        RenderUtil::DestroyVkObject(pQueryPool, vkDestroyQueryPool, this->m_handle);
    }

    VkDescriptorPool CGraphicsDevice::CreateDescriptorPool(VkDescriptorPoolCreateInfo* pCreateInfo, const char* name) const
    {
        VkDescriptorPool result{};
        RenderUtil::VkCheck(vkCreateDescriptorPool(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_DESCRIPTOR_POOL, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroyDescriptorPool(VkDescriptorPool* pPool)
    {
        RenderUtil::DestroyVkObject(pPool, vkDestroyDescriptorPool, this->m_handle);
    }

    void CGraphicsDevice::BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset)
    {
        RenderUtil::VkCheck(vkBindBufferMemory(this->m_handle, buffer, memory, memoryOffset));
    }

    void CGraphicsDevice::BindBufferMemory2(uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos)
    {
        RenderUtil::VkCheck(vkBindBufferMemory2(this->m_handle, bindInfoCount, pBindInfos));
    }

    void CGraphicsDevice::BindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset)
    {
        RenderUtil::VkCheck(vkBindImageMemory(this->m_handle, image, memory, memoryOffset));
    }

    void CGraphicsDevice::BindImageMemory2(uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos)
    {
        RenderUtil::VkCheck(vkBindImageMemory2(this->m_handle, bindInfoCount, pBindInfos));
    }

    void CGraphicsDevice::FreeMemory(VkDeviceMemory memory)
    {
        vkFreeMemory(this->m_handle, memory, nullptr);
    }

    VkResult CGraphicsDevice::AcquireNextImageKHR(VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pIndex)
    {
        return vkAcquireNextImageKHR(this->m_handle, swapchain, timeout, semaphore, fence, pIndex);
    }

    VkResult CGraphicsDevice::AcquireNextImage2KHR(VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pIndex)
    {
        return vkAcquireNextImage2KHR(this->m_handle, pAcquireInfo, pIndex);
    }

    VkSwapchainKHR CGraphicsDevice::CreateSwapchainKHR(VkSwapchainCreateInfoKHR* pCreateInfo, const char* name) const
    {
        VkSwapchainKHR result{};
        RenderUtil::VkCheck(vkCreateSwapchainKHR(this->m_handle, pCreateInfo, nullptr, &result));
        YGG_VK_DEBUG_NAME(this->m_handle, result, VK_OBJECT_TYPE_SWAPCHAIN_KHR, name);
        YGG_ASSERT(result);
        return result;
    }

    void CGraphicsDevice::DestroySwapchainKHR(VkSwapchainKHR* pSwapchain)
    {
        RenderUtil::DestroyVkObject(pSwapchain, vkDestroySwapchainKHR, this->m_handle);
    }

    void CGraphicsDevice::WaitIdle() const
    {
        RenderUtil::VkCheck(vkDeviceWaitIdle(this->m_handle));
    }
}
