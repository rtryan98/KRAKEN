#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "Yggdrasil/Defines.h"

namespace yggdrasil::vulkan::util
{
    const char* toString(VkResult result);

    VkSemaphore createSemaphore(VkDevice device);

    VkFence createFence(VkDevice device, VkFenceCreateFlags flags);

    std::vector<char> parseSPIRV(const char* path);
}

#if YGGDRASIL_USE_ASSERTS
    #define VK_CHECK(fun)                                                                                \
    do {                                                                                                 \
        VkResult vkCheckResult{ fun };                                                                   \
        YGGDRASIL_ASSERT_MSG( vkCheckResult == VK_SUCCESS, yggdrasil::vulkan::util::toString(vkCheckResult) ); \
    } while( 0 )
#else
    #define VK_CHECK(fun) fun
#endif
