// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#include "Yggdrasil/Common/Util/Logger.h"

#include <vulkan/vulkan.h>

namespace Ygg::RenderUtil
{
    template<typename T>
    void DestroyVkObject(T* handle, VKAPI_ATTR void(VKAPI_CALL* destroyFunction)(VkDevice, T, const VkAllocationCallbacks*), VkDevice device)
    {
        YGG_ASSERT(handle != nullptr);
        if (*handle != VK_NULL_HANDLE)
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

    void InitDebugExtensions(VkInstance instance);
    void SetVkObjectDebugName(VkDevice device, uint64_t handle, VkObjectType type, const char* name);
}

#if YGG_USE_ASSERTS
    #define YGG_VK_DEBUG_NAME(device, handle, type, name) \
    if(name != nullptr) \
        Ygg::RenderUtil::SetVkObjectDebugName(device, reinterpret_cast<uint64_t>(handle), type, name)
#else
    #define YGG_VK_DEBUG_NAME(device, handle, type, name)
#endif
