#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/RenderUtil.h"

#include <Windows.h>

namespace Ygg
{
    const char* ToString(VkResult result)
    {
        switch (result)
        {
        case VK_SUCCESS:                                            return "VK_SUCCESS";
        case VK_NOT_READY:                                          return "VK_NOT_READY";
        case VK_TIMEOUT:                                            return "VK_TIMEOUT";
        case VK_EVENT_SET:                                          return "VK_EVENT_SET";
        case VK_EVENT_RESET:                                        return "VK_EVENT_RESET";
        case VK_INCOMPLETE:                                         return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:                           return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:                         return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:                        return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:                                  return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:                            return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:                            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:                        return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:                          return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:                          return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:                             return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:                         return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:                              return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN:                                      return "VK_ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY:                           return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:                      return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION:                                return "VK_ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:               return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_ERROR_SURFACE_LOST_KHR:                             return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:                     return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:                                     return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:                              return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:                     return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:                        return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:                            return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case VK_ERROR_NOT_PERMITTED_EXT:                            return "VK_ERROR_NOT_PERMITTED_EXT";
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:          return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case VK_THREAD_IDLE_KHR:                                    return "VK_THREAD_IDLE_KHR";
        case VK_THREAD_DONE_KHR:                                    return "VK_THREAD_DONE_KHR";
        case VK_OPERATION_DEFERRED_KHR:                             return "VK_OPERATION_DEFERRED_KHR";
        case VK_OPERATION_NOT_DEFERRED_KHR:                         return "VK_OPERATION_NOT_DEFERRED_KHR";
        case VK_PIPELINE_COMPILE_REQUIRED_EXT:                      return "VK_PIPELINE_COMPILE_REQUIRED_EXT";
        default:                                                    return "UNKNOWN VULKAN ERROR";
        }
    }

    const char* VkDeviceFeatures10ToString(uint32_t feature)
    {
        switch (feature)
        {
        case  0: return "robustBufferAccess";
        case  1: return "fullDrawIndexUint32";
        case  2: return "imageCubeArray";
        case  3: return "independentBlend";
        case  4: return "geometryShader";
        case  5: return "tessellationShader";
        case  6: return "sampleRateShading";
        case  7: return "dualSrcBlend";
        case  8: return "logicOp";
        case  9: return "multiDrawIndirect";
        case 10: return "drawIndirectFirstInstance";
        case 11: return "depthClamp";
        case 12: return "depthBiasClamp";
        case 13: return "fillModeNonSolid";
        case 14: return "depthBounds";
        case 15: return "wideLines";
        case 16: return "largePoints";
        case 17: return "alphaToOne";
        case 18: return "multiViewport";
        case 19: return "samplerAnisotropy";
        case 20: return "textureCompressionETC2";
        case 21: return "textureCompressionASTC_LDR";
        case 22: return "textureCompressionBC";
        case 23: return "occlusionQueryPrecise";
        case 24: return "pipelineStatisticsQuery";
        case 25: return "vertexPipelineStoresAndAtomics";
        case 26: return "fragmentStoresAndAtomics";
        case 27: return "shaderTessellationAndGeometryPointSize";
        case 28: return "shaderImageGatherExtended";
        case 29: return "shaderStorageImageExtendedFormats";
        case 30: return "shaderStorageImageMultisample";
        case 31: return "shaderStorageImageReadWithoutFormat";
        case 32: return "shaderStorageImageWriteWithoutFormat";
        case 33: return "shaderUniformBufferArrayDynamicIndexing";
        case 34: return "shaderSampledImageArrayDynamicIndexing";
        case 35: return "shaderStorageBufferArrayDynamicIndexing";
        case 36: return "shaderStorageImageArrayDynamicIndexing";
        case 37: return "shaderClipDistance";
        case 38: return "shaderCullDistance";
        case 39: return "shaderFloat64";
        case 40: return "shaderInt64";
        case 41: return "shaderInt16";
        case 42: return "shaderResourceResidency";
        case 43: return "shaderResourceMinLod";
        case 44: return "sparseBinding";
        case 45: return "sparseResidencyBuffer";
        case 46: return "sparseResidencyImage2D";
        case 47: return "sparseResidencyImage3D";
        case 48: return "sparseResidency2Samples";
        case 49: return "sparseResidency4Samples";
        case 50: return "sparseResidency8Samples";
        case 51: return "sparseResidency16Samples";
        case 52: return "sparseResidencyAliased";
        case 53: return "variableMultisampleRate";
        case 54: return "inheritedQueries";
        default: return "";
        }
    }

    const char* VkDeviceFeatures11ToString(uint32_t feature)
    {
        switch (feature)
        {
        case  0: return "storageBuffer16BitAccess";
        case  1: return "uniformAndStorageBuffer16BitAccess";
        case  2: return "storagePushConstant16";
        case  3: return "storageInputOutput16";
        case  4: return "multiview";
        case  5: return "multiviewGeometryShader";
        case  6: return "multiviewTessellationShader";
        case  7: return "variablePointersStorageBuffer";
        case  8: return "variablePointers";
        case  9: return "protectedMemory";
        case 10: return "samplerYcbcrConversion";
        case 11: return "shaderDrawParameters";
        default: return "";
        }
    }

    const char* VkDeviceFeatures12ToString(uint32_t feature)
    {
        switch (feature)
        {
        case  0: return "samplerMirrorClampToEdge";
        case  1: return "drawIndirectCount";
        case  2: return "storageBuffer8BitAccess";
        case  3: return "uniformAndStorageBuffer8BitAccess";
        case  4: return "storagePushConstant8";
        case  5: return "shaderBufferInt64Atomics";
        case  6: return "shaderSharedInt64Atomics";
        case  7: return "shaderFloat16";
        case  8: return "shaderInt8";
        case  9: return "descriptorIndexing";
        case 10: return "shaderInputAttachmentArrayDynamicIndexing";
        case 11: return "shaderUniformTexelBufferArrayDynamicIndexing";
        case 12: return "shaderStorageTexelBufferArrayDynamicIndexing";
        case 13: return "shaderUniformBufferArrayNonUniformIndexing";
        case 14: return "shaderSampledImageArrayNonUniformIndexing";
        case 15: return "shaderStorageBufferArrayNonUniformIndexing";
        case 16: return "shaderStorageImageArrayNonUniformIndexing";
        case 17: return "shaderInputAttachmentArrayNonUniformIndexing";
        case 18: return "shaderUniformTexelBufferArrayNonUniformIndexing";
        case 19: return "shaderStorageTexelBufferArrayNonUniformIndexing";
        case 20: return "descriptorBindingUniformBufferUpdateAfterBind";
        case 21: return "descriptorBindingSampledImageUpdateAfterBind";
        case 22: return "descriptorBindingStorageImageUpdateAfterBind";
        case 23: return "descriptorBindingStorageBufferUpdateAfterBind";
        case 24: return "descriptorBindingUniformTexelBufferUpdateAfterBind";
        case 25: return "descriptorBindingStorageTexelBufferUpdateAfterBind";
        case 26: return "descriptorBindingUpdateUnusedWhilePending";
        case 27: return "descriptorBindingPartiallyBound";
        case 28: return "descriptorBindingVariableDescriptorCount";
        case 29: return "runtimeDescriptorArray";
        case 30: return "samplerFilterMinmax";
        case 31: return "scalarBlockLayout";
        case 32: return "imagelessFramebuffer";
        case 33: return "uniformBufferStandardLayout";
        case 34: return "shaderSubgroupExtendedTypes";
        case 35: return "separateDepthStencilLayouts";
        case 36: return "hostQueryReset";
        case 37: return "timelineSemaphore";
        case 38: return "bufferDeviceAddress";
        case 39: return "bufferDeviceAddressCaptureReplay";
        case 40: return "bufferDeviceAddressMultiDevice";
        case 41: return "vulkanMemoryModel";
        case 42: return "vulkanMemoryModelDeviceScope";
        case 43: return "vulkanMemoryModelAvailabilityVisibilityChains";
        case 44: return "shaderOutputViewportIndex";
        case 45: return "shaderOutputLayer";
        case 46: return "subgroupBroadcastDynamicId";
        default: return "";
        }
    }

    void VkCheck(VkResult result)
    {
        do
        {
            if (result != VK_SUCCESS)
            {
                Ygg::Logger::validationLogger->critical(ToString(result));
            }
        } while (0);
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    )
    {
        { // unused variables
            type;
            pUserData;
        }
        if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            Ygg::Logger::validationLogger->error("{0}", pCallbackData->pMessage);
            DebugBreak();
        }
        else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            Ygg::Logger::validationLogger->warn("{0}", pCallbackData->pMessage);
        }
        else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            Ygg::Logger::validationLogger->trace("{0}", pCallbackData->pMessage);
        }
        return VK_FALSE;
    }

    VkDebugUtilsMessengerEXT debugMessenger{};

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;

    void LoadVkDebugUtilsFunctions(VkInstance instance)
    {
        vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    }

    void CreateDebugMessenger(VkInstance instance)
    {
#if YGG_USE_ASSERTS
        Ygg::Logger::validationLogger->info("Enabled Vulkan Debug Messenger.");
        LoadVkDebugUtilsFunctions(instance);

        VkDebugUtilsMessengerCreateInfoEXT createInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData = nullptr;

        VkCheck(vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger));
#endif
    }

    void DestroyDebugMessenger(VkInstance instance)
    {
        vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT{};
    PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT{};
    PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT{};
    PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT{};

    void InitDebugExtensions(VkInstance instance)
    {
        vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT"));
        vkCmdDebugMarkerBeginEXT = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetInstanceProcAddr(instance, "vkCmdDebugMarkerBeginEXT"));
        vkCmdDebugMarkerEndEXT = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(vkGetInstanceProcAddr(instance, "vkCmdDebugMarkerEndEXT"));
        vkCmdDebugMarkerInsertEXT = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(vkGetInstanceProcAddr(instance, "vkCmdDebugMarkerInsertEXT"));
    }

    void SetVkObjectDebugName(VkDevice device, uint64_t handle, VkObjectType type, const char* name)
    {
        VkDebugUtilsObjectNameInfoEXT info{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
        info.objectType = type;
        info.objectHandle = handle;
        info.pObjectName = name;
        vkSetDebugUtilsObjectNameEXT(device, &info);
    }
}
