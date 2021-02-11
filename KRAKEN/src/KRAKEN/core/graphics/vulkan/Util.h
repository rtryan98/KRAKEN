#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "KRAKEN/Defines.h"

namespace kraken::vulkan::util
{
    const char* toString(VkResult result);

    VkSemaphore createSemaphore(VkDevice device);

    VkFence createFence(VkDevice device, VkFenceCreateFlags flags);

    std::vector<char> parseSPIRV(const char* path);
}

#if KRAKEN_USE_ASSERTS
    #define VK_CHECK(fun)                                                                                \
    do {                                                                                                 \
        VkResult vkCheckResult{ fun };                                                                   \
        KRAKEN_ASSERT_MSG( vkCheckResult == VK_SUCCESS, kraken::vulkan::util::toString(vkCheckResult) ); \
    } while( 0 )
#else
    #define VK_CHECK(fun) fun
#endif
