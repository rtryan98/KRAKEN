#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "Yggdrasil/Defines.h"

namespace yggdrasil::graphics::util
{
    const char* toString(VkResult result);

    VkSemaphore createSemaphore(VkDevice device);

    VkFence createFence(VkDevice device, VkFenceCreateFlags flags);

    std::vector<char> parseSPIRV(const char* path);

    void createDebugMessenger(VkInstance instance);
    void freeDebugMessenger(VkInstance instance);
}

#if YGGDRASIL_USE_ASSERTS
    #define VK_CHECK(fun)                                                                                        \
    do {                                                                                                         \
        VkResult vkCheckResult{ fun };                                                                           \
        YGGDRASIL_ASSERT_MSG( vkCheckResult == VK_SUCCESS, yggdrasil::graphics::util::toString(vkCheckResult) ); \
    } while( 0 )
#else
    #define VK_CHECK(fun) fun
#endif
