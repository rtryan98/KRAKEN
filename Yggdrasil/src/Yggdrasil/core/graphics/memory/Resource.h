#pragma once
#include "Yggdrasil/core/graphics/Device.h"
#include <vulkan/vulkan.h>

namespace yggdrasil::graphics::memory
{
    struct AllocatedBuffer
    {
        VkBuffer buffer{};
        VkDeviceMemory memory{};
        uint64_t size{};
        VkDeviceSize deviceSize{};
    };

    AllocatedBuffer createAllocatedBuffer(Device& device, uint64_t size, VkMemoryPropertyFlags flags, VkBufferUsageFlags usageFlags);
    void destroyAllocatedBuffer(Device& device, AllocatedBuffer& buffer);
    void uploadDataToBuffer(Device& device, AllocatedBuffer& dst, void* data, uint64_t offset, uint64_t size);
    void copyAllocatedBuffer(Device& device, AllocatedBuffer& src, AllocatedBuffer& dst, VkCommandPool commandPool, VkCommandBuffer commandBuffer);
}
