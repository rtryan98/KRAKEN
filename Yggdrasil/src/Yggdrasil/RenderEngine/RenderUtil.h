#pragma once
#include "Yggdrasil/Common/Util/Logger.h"

#include <vulkan/vulkan.h>

namespace Ygg
{
    template<typename T>
    void DestroyVkObject(T* handle, VKAPI_ATTR void(VKAPI_CALL* destroyFunction)(VkDevice, T, const VkAllocationCallbacks*), VkDevice device)
    {
        if (handle != VK_NULL_HANDLE)
        {
            destroyFunction(device, *handle, nullptr);
            *handle = VK_NULL_HANDLE;
        }
    }

    const char* ToString(VkResult result);
    const char* VkDeviceFeatures10ToString(uint32_t feature);
    const char* VkDeviceFeatures11ToString(uint32_t feature);
    const char* VkDeviceFeatures12ToString(uint32_t feature);

    void VkCheck(VkResult result);

    void CreateDebugMessenger(VkInstance instance);
    void DestroyDebugMessenger(VkInstance instance);
}
