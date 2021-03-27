#pragma once
#include "Yggdrasil/Defines.h"
#include "Yggdrasil/core/graphics/Device.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>

namespace ygg::graphics::util
{
    const char* toString(VkResult result);

    VkSemaphore createSemaphore(VkDevice device);

    VkFence createFence(VkDevice device, VkFenceCreateFlags flags);

    std::vector<char> parseSPIRV(const char* path);

    void createDebugMessenger(VkInstance instance);
    void freeDebugMessenger(VkInstance instance);

    void initDebugExtensions(VkInstance instance);

    void setObjectDebugName(const Device& device, uint64_t handle, VkObjectType type, const char* name);
    void beginDebugRegion(VkCommandBuffer commandBuffer, const char* name, glm::vec4 color);
    void insertDebugMarker(VkCommandBuffer commandBuffer, const char* name, glm::vec4 color);
    void endDebugRegion(VkCommandBuffer commandBuffer);

    template<typename T>
    void destroy(T* handle, VKAPI_ATTR void(VKAPI_CALL* destroyFunction)(VkDevice, T, const VkAllocationCallbacks*), VkDevice device)
    {
        if (handle != VK_NULL_HANDLE)
        {
            destroyFunction(device, *handle, VK_CPU_ALLOCATOR);
            *handle = VK_NULL_HANDLE;
        }
    }
}

#if YGGDRASIL_USE_ASSERTS
    #define VK_CHECK(fun)                                                                                        \
    do {                                                                                                         \
        VkResult vkCheckResult{ fun };                                                                           \
        YGGDRASIL_ASSERT_MSG( vkCheckResult == VK_SUCCESS, ygg::graphics::util::toString(vkCheckResult) ); \
    } while( 0 )

    #define VK_SET_OBJECT_DEBUG_NAME( device, handle, type, name )                                               \
    ygg::graphics::util::setObjectDebugName(device, handle, type, name)

    #define VK_BEGIN_DEBUG_REGION( cmdbuf, name, col )                                                           \
    ygg::graphics::util::beginDebugRegion(cmdbuf, name, col)

    #define VK_INSERT_DEBUG_MARKER( cmdbuf, name, col )                                                          \
    ygg::graphics::util::insertDebugMarker( cmdbuf, name, col )

    #define VK_END_DEBUG_REGION( cmdbuf )                                                                        \
    ygg::graphics::util::endDebugRegion( cmdbuf )
#else
    #define VK_CHECK(fun) fun
    #define VK_SET_OBJECT_DEBUG_NAME( device, handle, type, name )
    #define VK_BEGIN_DEBUG_REGION( cmdbuf, name, col )
    #define VK_INSERT_DEBUG_MARKER( cmdbuf, name, col )
    #define VK_END_DEBUG_REGION( cmdbuf )
#endif
