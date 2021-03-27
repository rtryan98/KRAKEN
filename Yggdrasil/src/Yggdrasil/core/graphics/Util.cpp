#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/graphics/Util.h"
#include "Yggdrasil/core/graphics/Globals.h"
#include <fstream>
#include <sstream>

namespace ygg::graphics::util
{
    const char* toString(VkResult result)
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

    VkSemaphore createSemaphore(VkDevice device)
    {
        VkSemaphore result{};
        VkSemaphoreCreateInfo semaphoreCreateInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, VK_CPU_ALLOCATOR, &result));
        YGGDRASIL_ASSERT_VALUE(result);
        return result;
    }

    VkFence createFence(VkDevice device, VkFenceCreateFlags flags)
    {
        VkFence result{};
        VkFenceCreateInfo fenceCreateInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        fenceCreateInfo.flags = flags;
        VK_CHECK(vkCreateFence(device, &fenceCreateInfo, VK_CPU_ALLOCATOR, &result));
        YGGDRASIL_ASSERT_VALUE(result);
        return result;
    }

    std::vector<char> parseSPIRV(const char* path)
    {
        std::ifstream ifStream{ path, std::ios::ate | std::ios::binary };
        YGGDRASIL_ASSERT_VALUE(ifStream.is_open());
        uint32_t fileSize{ static_cast<uint32_t>(ifStream.tellg()) };
        std::vector<char> result(fileSize);
        ifStream.seekg(0);
        ifStream.read(result.data(), fileSize);
        ifStream.close();
        return result;
    }

    VkDebugUtilsMessengerEXT debugMessenger{};
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT{};
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT{};

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    )
    {
        YGGDRASIL_UNUSED_VARIABLE(pUserData);
        YGGDRASIL_UNUSED_VARIABLE(type);
        if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            ::ygg::Logger::getValidationErrorLogger()->error("{0}", pCallbackData->pMessage);
            debugBreak();
        }
        else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            ::ygg::Logger::getValidationErrorLogger()->warn("{0}", pCallbackData->pMessage);
        }
        else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            ::ygg::Logger::getValidationErrorLogger()->trace("{0}", pCallbackData->pMessage);
        }
        return VK_FALSE;
    }

    void createDebugMessenger(VkInstance instance)
    {
        vkCreateDebugUtilsMessengerEXT  = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

        VkDebugUtilsMessengerCreateInfoEXT createInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;

        VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance, &createInfo, VK_CPU_ALLOCATOR, &debugMessenger));
        YGGDRASIL_ASSERT_VALUE(debugMessenger);
    }

    void freeDebugMessenger(VkInstance instance)
    {
        if (debugMessenger != VK_NULL_HANDLE)
        {
            vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, VK_CPU_ALLOCATOR);
        }
    }

    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT{};
    PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT{};
    PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT{};
    PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT{};

    void initDebugExtensions(VkInstance instance)
    {
        vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT"));
        vkCmdDebugMarkerBeginEXT     = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT    >(vkGetInstanceProcAddr(instance, "vkCmdDebugMarkerBeginEXT"));
        vkCmdDebugMarkerEndEXT       = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT      >(vkGetInstanceProcAddr(instance, "vkCmdDebugMarkerEndEXT"));
        vkCmdDebugMarkerInsertEXT    = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT   >(vkGetInstanceProcAddr(instance, "vkCmdDebugMarkerInsertEXT"));
    }

    void setObjectDebugName(const Device& device, uint64_t handle, VkObjectType type, const char* name)
    {
        VkDebugUtilsObjectNameInfoEXT info{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
        info.objectType = type;
        info.objectHandle = handle;
        info.pObjectName = name;
        vkSetDebugUtilsObjectNameEXT(device.logical, &info);
    }

    void beginDebugRegion(VkCommandBuffer commandBuffer, const char* name, glm::vec4 color)
    {
        VkDebugMarkerMarkerInfoEXT info{ VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT };
        memcpy(info.color, &color[0], sizeof(float_t) * 4);
        info.pMarkerName = name;
        vkCmdDebugMarkerBeginEXT(commandBuffer, &info);
    }

    void insertDebugMarker(VkCommandBuffer commandBuffer, const char* name, glm::vec4 color)
    {
        VkDebugMarkerMarkerInfoEXT info{ VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT };
        memcpy(info.color, &color[0], sizeof(float_t) * 4);
        info.pMarkerName = name;
        vkCmdDebugMarkerInsertEXT(commandBuffer, &info);
    }

    void endDebugRegion(VkCommandBuffer commandBuffer)
    {
        vkCmdDebugMarkerEndEXT(commandBuffer);
    }
}
