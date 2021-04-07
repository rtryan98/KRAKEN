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
            YGG_INFO("Enabling enableAllFeatures available Vulkan features. This may impact performance.");
            this->enabledVulkan10Features = availableVulkan10Features;
            this->enabledVulkan11Features = availableVulkan11Features;
            this->enabledVulkan12Features = availableVulkan12Features;
        }
        else
        {
            if (pRequestedVulkan10Features != nullptr)
            {
                if (pRequestedVulkan10Features->robustBufferAccess && availableVulkan10Features.features.robustBufferAccess)
                    this->enabledVulkan10Features.features.robustBufferAccess = true;

                if (pRequestedVulkan10Features->fullDrawIndexUint32 && availableVulkan10Features.features.fullDrawIndexUint32)
                    this->enabledVulkan10Features.features.fullDrawIndexUint32 = true;

                if (pRequestedVulkan10Features->imageCubeArray && availableVulkan10Features.features.imageCubeArray)
                    this->enabledVulkan10Features.features.imageCubeArray = true;

                if (pRequestedVulkan10Features->independentBlend && availableVulkan10Features.features.independentBlend)
                    this->enabledVulkan10Features.features.independentBlend = true;

                if (pRequestedVulkan10Features->geometryShader && availableVulkan10Features.features.geometryShader)
                    this->enabledVulkan10Features.features.geometryShader = true;

                if (pRequestedVulkan10Features->tessellationShader && availableVulkan10Features.features.tessellationShader)
                    this->enabledVulkan10Features.features.tessellationShader = true;

                if (pRequestedVulkan10Features->sampleRateShading && availableVulkan10Features.features.sampleRateShading)
                    this->enabledVulkan10Features.features.sampleRateShading = true;

                if (pRequestedVulkan10Features->dualSrcBlend && availableVulkan10Features.features.dualSrcBlend)
                    this->enabledVulkan10Features.features.dualSrcBlend = true;

                if (pRequestedVulkan10Features->logicOp && availableVulkan10Features.features.logicOp)
                    this->enabledVulkan10Features.features.logicOp = true;

                if (pRequestedVulkan10Features->multiDrawIndirect && availableVulkan10Features.features.multiDrawIndirect)
                    this->enabledVulkan10Features.features.multiDrawIndirect = true;

                if (pRequestedVulkan10Features->drawIndirectFirstInstance && availableVulkan10Features.features.drawIndirectFirstInstance)
                    this->enabledVulkan10Features.features.drawIndirectFirstInstance = true;

                if (pRequestedVulkan10Features->depthClamp && availableVulkan10Features.features.depthClamp)
                    this->enabledVulkan10Features.features.depthClamp = true;

                if (pRequestedVulkan10Features->depthBiasClamp && availableVulkan10Features.features.depthBiasClamp)
                    this->enabledVulkan10Features.features.depthBiasClamp = true;

                if (pRequestedVulkan10Features->fillModeNonSolid && availableVulkan10Features.features.fillModeNonSolid)
                    this->enabledVulkan10Features.features.fillModeNonSolid = true;

                if (pRequestedVulkan10Features->depthBounds && availableVulkan10Features.features.depthBounds)
                    this->enabledVulkan10Features.features.depthBounds = true;

                if (pRequestedVulkan10Features->wideLines && availableVulkan10Features.features.wideLines)
                    this->enabledVulkan10Features.features.wideLines = true;

                if (pRequestedVulkan10Features->largePoints && availableVulkan10Features.features.largePoints)
                    this->enabledVulkan10Features.features.largePoints = true;

                if (pRequestedVulkan10Features->alphaToOne && availableVulkan10Features.features.alphaToOne)
                    this->enabledVulkan10Features.features.alphaToOne = true;

                if (pRequestedVulkan10Features->multiViewport && availableVulkan10Features.features.multiViewport)
                    this->enabledVulkan10Features.features.multiViewport = true;

                if (pRequestedVulkan10Features->samplerAnisotropy && availableVulkan10Features.features.samplerAnisotropy)
                    this->enabledVulkan10Features.features.samplerAnisotropy = true;

                if (pRequestedVulkan10Features->textureCompressionETC2 && availableVulkan10Features.features.textureCompressionETC2)
                    this->enabledVulkan10Features.features.textureCompressionETC2 = true;

                if (pRequestedVulkan10Features->textureCompressionASTC_LDR && availableVulkan10Features.features.textureCompressionASTC_LDR)
                    this->enabledVulkan10Features.features.textureCompressionASTC_LDR = true;

                if (pRequestedVulkan10Features->textureCompressionBC && availableVulkan10Features.features.textureCompressionBC)
                    this->enabledVulkan10Features.features.textureCompressionBC = true;

                if (pRequestedVulkan10Features->occlusionQueryPrecise && availableVulkan10Features.features.occlusionQueryPrecise)
                    this->enabledVulkan10Features.features.occlusionQueryPrecise = true;

                if (pRequestedVulkan10Features->pipelineStatisticsQuery && availableVulkan10Features.features.pipelineStatisticsQuery)
                    this->enabledVulkan10Features.features.pipelineStatisticsQuery = true;

                if (pRequestedVulkan10Features->vertexPipelineStoresAndAtomics && availableVulkan10Features.features.vertexPipelineStoresAndAtomics)
                    this->enabledVulkan10Features.features.vertexPipelineStoresAndAtomics = true;

                if (pRequestedVulkan10Features->fragmentStoresAndAtomics && availableVulkan10Features.features.fragmentStoresAndAtomics)
                    this->enabledVulkan10Features.features.fragmentStoresAndAtomics = true;

                if (pRequestedVulkan10Features->shaderTessellationAndGeometryPointSize && availableVulkan10Features.features.shaderTessellationAndGeometryPointSize)
                    this->enabledVulkan10Features.features.shaderTessellationAndGeometryPointSize = true;

                if (pRequestedVulkan10Features->shaderImageGatherExtended && availableVulkan10Features.features.shaderImageGatherExtended)
                    this->enabledVulkan10Features.features.shaderImageGatherExtended = true;

                if (pRequestedVulkan10Features->shaderStorageImageExtendedFormats && availableVulkan10Features.features.shaderStorageImageExtendedFormats)
                    this->enabledVulkan10Features.features.shaderStorageImageExtendedFormats = true;

                if (pRequestedVulkan10Features->shaderStorageImageMultisample && availableVulkan10Features.features.shaderStorageImageMultisample)
                    this->enabledVulkan10Features.features.shaderStorageImageMultisample = true;

                if (pRequestedVulkan10Features->shaderStorageImageReadWithoutFormat && availableVulkan10Features.features.shaderStorageImageReadWithoutFormat)
                    this->enabledVulkan10Features.features.shaderStorageImageReadWithoutFormat = true;

                if (pRequestedVulkan10Features->shaderStorageImageWriteWithoutFormat && availableVulkan10Features.features.shaderStorageImageWriteWithoutFormat)
                    this->enabledVulkan10Features.features.shaderStorageImageWriteWithoutFormat = true;

                if (pRequestedVulkan10Features->shaderUniformBufferArrayDynamicIndexing && availableVulkan10Features.features.shaderUniformBufferArrayDynamicIndexing)
                    this->enabledVulkan10Features.features.shaderUniformBufferArrayDynamicIndexing = true;

                if (pRequestedVulkan10Features->shaderSampledImageArrayDynamicIndexing && availableVulkan10Features.features.shaderSampledImageArrayDynamicIndexing)
                    this->enabledVulkan10Features.features.shaderSampledImageArrayDynamicIndexing = true;

                if (pRequestedVulkan10Features->shaderStorageBufferArrayDynamicIndexing && availableVulkan10Features.features.shaderStorageBufferArrayDynamicIndexing)
                    this->enabledVulkan10Features.features.shaderStorageBufferArrayDynamicIndexing = true;

                if (pRequestedVulkan10Features->shaderStorageImageArrayDynamicIndexing && availableVulkan10Features.features.shaderStorageImageArrayDynamicIndexing)
                    this->enabledVulkan10Features.features.shaderStorageImageArrayDynamicIndexing = true;

                if (pRequestedVulkan10Features->shaderClipDistance && availableVulkan10Features.features.shaderClipDistance)
                    this->enabledVulkan10Features.features.shaderClipDistance = true;

                if (pRequestedVulkan10Features->shaderCullDistance && availableVulkan10Features.features.shaderCullDistance)
                    this->enabledVulkan10Features.features.shaderCullDistance = true;

                if (pRequestedVulkan10Features->shaderFloat64 && availableVulkan10Features.features.shaderFloat64)
                    this->enabledVulkan10Features.features.shaderFloat64 = true;

                if (pRequestedVulkan10Features->shaderInt64 && availableVulkan10Features.features.shaderInt64)
                    this->enabledVulkan10Features.features.shaderInt64 = true;

                if (pRequestedVulkan10Features->shaderInt16 && availableVulkan10Features.features.shaderInt16)
                    this->enabledVulkan10Features.features.shaderInt16 = true;

                if (pRequestedVulkan10Features->shaderResourceResidency && availableVulkan10Features.features.shaderResourceResidency)
                    this->enabledVulkan10Features.features.shaderResourceResidency = true;

                if (pRequestedVulkan10Features->shaderResourceMinLod && availableVulkan10Features.features.shaderResourceMinLod)
                    this->enabledVulkan10Features.features.shaderResourceMinLod = true;

                if (pRequestedVulkan10Features->sparseBinding && availableVulkan10Features.features.sparseBinding)
                    this->enabledVulkan10Features.features.sparseBinding = true;

                if (pRequestedVulkan10Features->sparseResidencyBuffer && availableVulkan10Features.features.sparseResidencyBuffer)
                    this->enabledVulkan10Features.features.sparseResidencyBuffer = true;

                if (pRequestedVulkan10Features->sparseResidencyImage2D && availableVulkan10Features.features.sparseResidencyImage2D)
                    this->enabledVulkan10Features.features.sparseResidencyImage2D = true;

                if (pRequestedVulkan10Features->sparseResidencyImage3D && availableVulkan10Features.features.sparseResidencyImage3D)
                    this->enabledVulkan10Features.features.sparseResidencyImage3D = true;

                if (pRequestedVulkan10Features->sparseResidency2Samples && availableVulkan10Features.features.sparseResidency2Samples)
                    this->enabledVulkan10Features.features.sparseResidency2Samples = true;

                if (pRequestedVulkan10Features->sparseResidency4Samples && availableVulkan10Features.features.sparseResidency4Samples)
                    this->enabledVulkan10Features.features.sparseResidency4Samples = true;

                if (pRequestedVulkan10Features->sparseResidency8Samples && availableVulkan10Features.features.sparseResidency8Samples)
                    this->enabledVulkan10Features.features.sparseResidency8Samples = true;

                if (pRequestedVulkan10Features->sparseResidency16Samples && availableVulkan10Features.features.sparseResidency16Samples)
                    this->enabledVulkan10Features.features.sparseResidency16Samples = true;

                if (pRequestedVulkan10Features->sparseResidencyAliased && availableVulkan10Features.features.sparseResidencyAliased)
                    this->enabledVulkan10Features.features.sparseResidencyAliased = true;

                if (pRequestedVulkan10Features->variableMultisampleRate && availableVulkan10Features.features.variableMultisampleRate)
                    this->enabledVulkan10Features.features.variableMultisampleRate = true;

                if (pRequestedVulkan10Features->inheritedQueries && availableVulkan10Features.features.inheritedQueries)
                    this->enabledVulkan10Features.features.inheritedQueries = true;
            }
            if (pRequestedVulkan11Features != nullptr)
            {
                if(pRequestedVulkan11Features->storageBuffer16BitAccess           && availableVulkan11Features.storageBuffer16BitAccess          )
                    this->enabledVulkan11Features.storageBuffer16BitAccess = true;

                if(pRequestedVulkan11Features->uniformAndStorageBuffer16BitAccess && availableVulkan11Features.uniformAndStorageBuffer16BitAccess)
                    this->enabledVulkan11Features.uniformAndStorageBuffer16BitAccess = true;

                if(pRequestedVulkan11Features->storagePushConstant16              && availableVulkan11Features.storagePushConstant16             )
                    this->enabledVulkan11Features.storagePushConstant16 = true;

                if(pRequestedVulkan11Features->storageInputOutput16               && availableVulkan11Features.storageInputOutput16              )
                    this->enabledVulkan11Features.storageInputOutput16 = true;

                if(pRequestedVulkan11Features->multiview                          && availableVulkan11Features.multiview                         )
                    this->enabledVulkan11Features.multiview = true;

                if(pRequestedVulkan11Features->multiviewGeometryShader            && availableVulkan11Features.multiviewGeometryShader           )
                    this->enabledVulkan11Features.multiviewGeometryShader = true;

                if(pRequestedVulkan11Features->multiviewTessellationShader        && availableVulkan11Features.multiviewTessellationShader       )
                    this->enabledVulkan11Features.multiviewTessellationShader = true;

                if(pRequestedVulkan11Features->variablePointersStorageBuffer      && availableVulkan11Features.variablePointersStorageBuffer     )
                    this->enabledVulkan11Features.variablePointersStorageBuffer = true;

                if(pRequestedVulkan11Features->variablePointers                   && availableVulkan11Features.variablePointers                  )
                    this->enabledVulkan11Features.variablePointers = true;

                if(pRequestedVulkan11Features->protectedMemory                    && availableVulkan11Features.protectedMemory                   )
                    this->enabledVulkan11Features.protectedMemory = true;

                if(pRequestedVulkan11Features->samplerYcbcrConversion             && availableVulkan11Features.samplerYcbcrConversion            )
                    this->enabledVulkan11Features.samplerYcbcrConversion = true;

                if(pRequestedVulkan11Features->shaderDrawParameters               && availableVulkan11Features.shaderDrawParameters              )
                    this->enabledVulkan11Features.shaderDrawParameters = true;

            }
            if (pRequestedVulkan12Features != nullptr)
            {
                if (pRequestedVulkan12Features->samplerMirrorClampToEdge && availableVulkan12Features.samplerMirrorClampToEdge)
                    this->enabledVulkan12Features.samplerMirrorClampToEdge = true;

                if(pRequestedVulkan12Features->drawIndirectCount                                  && availableVulkan12Features.drawIndirectCount                                 )
                    this->enabledVulkan12Features.drawIndirectCount = true;

                if(pRequestedVulkan12Features->storageBuffer8BitAccess                            && availableVulkan12Features.storageBuffer8BitAccess                           )
                    this->enabledVulkan12Features.storageBuffer8BitAccess = true;

                if(pRequestedVulkan12Features->uniformAndStorageBuffer8BitAccess                  && availableVulkan12Features.uniformAndStorageBuffer8BitAccess                 )
                    this->enabledVulkan12Features.uniformAndStorageBuffer8BitAccess = true;

                if(pRequestedVulkan12Features->storagePushConstant8                               && availableVulkan12Features.storagePushConstant8                              )
                    this->enabledVulkan12Features.storagePushConstant8 = true;

                if(pRequestedVulkan12Features->shaderBufferInt64Atomics                           && availableVulkan12Features.shaderBufferInt64Atomics                          )
                    this->enabledVulkan12Features.shaderBufferInt64Atomics = true;

                if(pRequestedVulkan12Features->shaderSharedInt64Atomics                           && availableVulkan12Features.shaderSharedInt64Atomics                          )
                    this->enabledVulkan12Features.shaderSharedInt64Atomics = true;

                if(pRequestedVulkan12Features->shaderFloat16                                      && availableVulkan12Features.shaderFloat16                                     )
                    this->enabledVulkan12Features.shaderFloat16 = true;

                if(pRequestedVulkan12Features->shaderInt8                                         && availableVulkan12Features.shaderInt8                                        )
                    this->enabledVulkan12Features.shaderInt8 = true;

                if(pRequestedVulkan12Features->descriptorIndexing                                 && availableVulkan12Features.descriptorIndexing                                )
                    this->enabledVulkan12Features.descriptorIndexing = true;

                if(pRequestedVulkan12Features->shaderInputAttachmentArrayDynamicIndexing          && availableVulkan12Features.shaderInputAttachmentArrayDynamicIndexing         )
                    this->enabledVulkan12Features.shaderInputAttachmentArrayDynamicIndexing = true;

                if(pRequestedVulkan12Features->shaderUniformTexelBufferArrayDynamicIndexing       && availableVulkan12Features.shaderUniformTexelBufferArrayDynamicIndexing      )
                    this->enabledVulkan12Features.shaderUniformTexelBufferArrayDynamicIndexing = true;

                if(pRequestedVulkan12Features->shaderStorageTexelBufferArrayDynamicIndexing       && availableVulkan12Features.shaderStorageTexelBufferArrayDynamicIndexing      )
                    this->enabledVulkan12Features.shaderStorageTexelBufferArrayDynamicIndexing = true;

                if(pRequestedVulkan12Features->shaderUniformBufferArrayNonUniformIndexing         && availableVulkan12Features.shaderUniformBufferArrayNonUniformIndexing        )
                    this->enabledVulkan12Features.shaderUniformBufferArrayNonUniformIndexing = true;

                if(pRequestedVulkan12Features->shaderSampledImageArrayNonUniformIndexing          && availableVulkan12Features.shaderSampledImageArrayNonUniformIndexing         )
                    this->enabledVulkan12Features.shaderSampledImageArrayNonUniformIndexing = true;

                if(pRequestedVulkan12Features->shaderStorageBufferArrayNonUniformIndexing         && availableVulkan12Features.shaderStorageBufferArrayNonUniformIndexing        )
                    this->enabledVulkan12Features.shaderStorageBufferArrayNonUniformIndexing = true;

                if(pRequestedVulkan12Features->shaderStorageImageArrayNonUniformIndexing          && availableVulkan12Features.shaderStorageImageArrayNonUniformIndexing         )
                    this->enabledVulkan12Features.shaderStorageImageArrayNonUniformIndexing = true;

                if(pRequestedVulkan12Features->shaderInputAttachmentArrayNonUniformIndexing       && availableVulkan12Features.shaderInputAttachmentArrayNonUniformIndexing      )
                    this->enabledVulkan12Features.shaderInputAttachmentArrayNonUniformIndexing = true;

                if(pRequestedVulkan12Features->shaderUniformTexelBufferArrayNonUniformIndexing    && availableVulkan12Features.shaderUniformTexelBufferArrayNonUniformIndexing   )
                    this->enabledVulkan12Features.shaderUniformTexelBufferArrayNonUniformIndexing = true;

                if(pRequestedVulkan12Features->shaderStorageTexelBufferArrayNonUniformIndexing    && availableVulkan12Features.shaderStorageTexelBufferArrayNonUniformIndexing   )
                    this->enabledVulkan12Features.shaderStorageTexelBufferArrayNonUniformIndexing = true;

                if(pRequestedVulkan12Features->descriptorBindingUniformBufferUpdateAfterBind      && availableVulkan12Features.descriptorBindingUniformBufferUpdateAfterBind     )
                    this->enabledVulkan12Features.descriptorBindingUniformBufferUpdateAfterBind = true;

                if(pRequestedVulkan12Features->descriptorBindingSampledImageUpdateAfterBind       && availableVulkan12Features.descriptorBindingSampledImageUpdateAfterBind      )
                    this->enabledVulkan12Features.descriptorBindingSampledImageUpdateAfterBind = true;

                if(pRequestedVulkan12Features->descriptorBindingStorageImageUpdateAfterBind       && availableVulkan12Features.descriptorBindingStorageImageUpdateAfterBind      )
                    this->enabledVulkan12Features.descriptorBindingStorageImageUpdateAfterBind = true;

                if(pRequestedVulkan12Features->descriptorBindingStorageBufferUpdateAfterBind      && availableVulkan12Features.descriptorBindingStorageBufferUpdateAfterBind     )
                    this->enabledVulkan12Features.descriptorBindingStorageBufferUpdateAfterBind = true;

                if(pRequestedVulkan12Features->descriptorBindingUniformTexelBufferUpdateAfterBind && availableVulkan12Features.descriptorBindingUniformTexelBufferUpdateAfterBind)
                    this->enabledVulkan12Features.descriptorBindingUniformTexelBufferUpdateAfterBind = true;

                if(pRequestedVulkan12Features->descriptorBindingStorageTexelBufferUpdateAfterBind && availableVulkan12Features.descriptorBindingStorageTexelBufferUpdateAfterBind)
                    this->enabledVulkan12Features.descriptorBindingStorageTexelBufferUpdateAfterBind = true;

                if(pRequestedVulkan12Features->descriptorBindingUpdateUnusedWhilePending          && availableVulkan12Features.descriptorBindingUpdateUnusedWhilePending         )
                    this->enabledVulkan12Features.descriptorBindingUpdateUnusedWhilePending = true;

                if(pRequestedVulkan12Features->descriptorBindingPartiallyBound                    && availableVulkan12Features.descriptorBindingPartiallyBound                   )
                    this->enabledVulkan12Features.descriptorBindingPartiallyBound = true;

                if(pRequestedVulkan12Features->descriptorBindingVariableDescriptorCount           && availableVulkan12Features.descriptorBindingVariableDescriptorCount          )
                    this->enabledVulkan12Features.descriptorBindingVariableDescriptorCount = true;

                if(pRequestedVulkan12Features->runtimeDescriptorArray                             && availableVulkan12Features.runtimeDescriptorArray                            )
                    this->enabledVulkan12Features.runtimeDescriptorArray = true;

                if(pRequestedVulkan12Features->samplerFilterMinmax                                && availableVulkan12Features.samplerFilterMinmax                               )
                    this->enabledVulkan12Features.samplerFilterMinmax = true;

                if(pRequestedVulkan12Features->scalarBlockLayout                                  && availableVulkan12Features.scalarBlockLayout                                 )
                    this->enabledVulkan12Features.scalarBlockLayout = true;

                if(pRequestedVulkan12Features->imagelessFramebuffer                               && availableVulkan12Features.imagelessFramebuffer                              )
                    this->enabledVulkan12Features.imagelessFramebuffer = true;

                if(pRequestedVulkan12Features->uniformBufferStandardLayout                        && availableVulkan12Features.uniformBufferStandardLayout                       )
                    this->enabledVulkan12Features.uniformBufferStandardLayout = true;

                if(pRequestedVulkan12Features->shaderSubgroupExtendedTypes                        && availableVulkan12Features.shaderSubgroupExtendedTypes                       )
                    this->enabledVulkan12Features.shaderSubgroupExtendedTypes = true;

                if(pRequestedVulkan12Features->separateDepthStencilLayouts                        && availableVulkan12Features.separateDepthStencilLayouts                       )
                    this->enabledVulkan12Features.separateDepthStencilLayouts = true;

                if(pRequestedVulkan12Features->hostQueryReset                                     && availableVulkan12Features.hostQueryReset                                    )
                    this->enabledVulkan12Features.hostQueryReset = true;

                if(pRequestedVulkan12Features->timelineSemaphore                                  && availableVulkan12Features.timelineSemaphore                                 )
                    this->enabledVulkan12Features.timelineSemaphore = true;

                if(pRequestedVulkan12Features->bufferDeviceAddress                                && availableVulkan12Features.bufferDeviceAddress                               )
                    this->enabledVulkan12Features.bufferDeviceAddress = true;

                if(pRequestedVulkan12Features->bufferDeviceAddressCaptureReplay                   && availableVulkan12Features.bufferDeviceAddressCaptureReplay                  )
                    this->enabledVulkan12Features.bufferDeviceAddressCaptureReplay = true;

                if(pRequestedVulkan12Features->bufferDeviceAddressMultiDevice                     && availableVulkan12Features.bufferDeviceAddressMultiDevice                    )
                    this->enabledVulkan12Features.bufferDeviceAddressMultiDevice = true;

                if(pRequestedVulkan12Features->vulkanMemoryModel                                  && availableVulkan12Features.vulkanMemoryModel                                 )
                    this->enabledVulkan12Features.vulkanMemoryModel = true;

                if(pRequestedVulkan12Features->vulkanMemoryModelDeviceScope                       && availableVulkan12Features.vulkanMemoryModelDeviceScope                      )
                    this->enabledVulkan12Features.vulkanMemoryModelDeviceScope = true;

                if(pRequestedVulkan12Features->vulkanMemoryModelAvailabilityVisibilityChains      && availableVulkan12Features.vulkanMemoryModelAvailabilityVisibilityChains     )
                    this->enabledVulkan12Features.vulkanMemoryModelAvailabilityVisibilityChains = true;

                if(pRequestedVulkan12Features->shaderOutputViewportIndex                          && availableVulkan12Features.shaderOutputViewportIndex                         )
                    this->enabledVulkan12Features.shaderOutputViewportIndex = true;

                if(pRequestedVulkan12Features->shaderOutputLayer                                  && availableVulkan12Features.shaderOutputLayer                                 )
                    this->enabledVulkan12Features.shaderOutputLayer = true;

                if(pRequestedVulkan12Features->subgroupBroadcastDynamicId                         && availableVulkan12Features.subgroupBroadcastDynamicId                        )
                    this->enabledVulkan12Features.subgroupBroadcastDynamicId = true;

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
