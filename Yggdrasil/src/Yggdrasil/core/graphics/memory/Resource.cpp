#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/memory/Resource.h"
#include "Yggdrasil/core/graphics/Util.h"
#include "Yggdrasil/core/graphics/Globals.h"

#include <vulkan/vulkan.h>

namespace yggdrasil::graphics::memory
{
    uint32_t selectMemoryType(Device& device, VkMemoryPropertyFlags flags)
    {
        auto& memoryProperties{ device.memory.properties };
        for (uint32_t i{ 0 }; i < memoryProperties.memoryTypeCount; i++)
        {
            auto& memoryType{ memoryProperties.memoryTypes[i] };
            if ((memoryType.propertyFlags & flags) == flags)
            {
                return i;
            }
        }
        YGGDRASIL_CORE_WARN("No valid memory type found.");
        return ~0u;
    }

    AllocatedBuffer createAllocatedBuffer(Device& device, uint64_t size, VkMemoryPropertyFlags flags, VkBufferUsageFlags usageFlags)
    {
        AllocatedBuffer result{};
        result.size = size;

        VkBufferCreateInfo bufferCreateInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferCreateInfo.size = size;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.queueFamilyIndexCount = 1;
        bufferCreateInfo.pQueueFamilyIndices = &device.queues.rasterizerQueueFamilyIndex;
        bufferCreateInfo.usage = usageFlags;
        bufferCreateInfo.flags = 0x0;

        VK_CHECK(vkCreateBuffer(device.logical, &bufferCreateInfo, VK_CPU_ALLOCATOR, &result.buffer));

        VkMemoryRequirements bufferMemoryRequirements{};
        vkGetBufferMemoryRequirements(device.logical, result.buffer, &bufferMemoryRequirements);
        result.deviceSize = bufferMemoryRequirements.size;

        VkMemoryAllocateInfo memoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        memoryAllocateInfo.allocationSize = bufferMemoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = selectMemoryType(device, flags);

        VK_CHECK(vkAllocateMemory(device.logical, &memoryAllocateInfo, VK_CPU_ALLOCATOR, &result.memory));
        VK_CHECK(vkBindBufferMemory(device.logical, result.buffer, result.memory, 0));

        return result;
    }

    void destroyAllocatedBuffer(Device& device, AllocatedBuffer& buffer)
    {
        if (buffer.buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device.logical, buffer.buffer, VK_CPU_ALLOCATOR);
        }
        if (buffer.memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device.logical, buffer.memory, VK_CPU_ALLOCATOR);
        }
    }

    void uploadDataToBuffer(Device& device, AllocatedBuffer& dst, void* data, uint64_t offset, uint64_t size)
    {
        void* bufferPtr{ nullptr };
        VK_CHECK(vkMapMemory(device.logical, dst.memory, offset, size, 0x0, &bufferPtr));
        memcpy(bufferPtr, data, size);
        vkUnmapMemory(device.logical, dst.memory);
    }

    void copyAllocatedBuffer(Device& device, AllocatedBuffer& src, AllocatedBuffer& dst, VkCommandPool commandPool, VkCommandBuffer commandBuffer)
    {
        VK_CHECK(vkResetCommandPool(device.logical, commandPool, 0x0));

        VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

        VkBufferCopy region{};
        region.dstOffset = 0;
        region.srcOffset = 0;
        region.size = src.size;

        vkCmdCopyBuffer(commandBuffer, src.buffer, dst.buffer, 1, &region);

        VkBufferMemoryBarrier barrier{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.offset = 0;
        barrier.size = VK_WHOLE_SIZE;
        barrier.buffer = dst.buffer;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_DEPENDENCY_BY_REGION_BIT,
            0, nullptr,
            1, &barrier,
            0, nullptr);

        VK_CHECK(vkEndCommandBuffer(commandBuffer));

        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VK_CHECK(vkQueueSubmit(device.queues.rasterizerQueue, 1, &submitInfo, VK_NULL_HANDLE));
        VK_CHECK(vkDeviceWaitIdle(device.logical));
    }
}
