#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/memory/UniformBuffer.h"

namespace yggdrasil::graphics::memory
{
    UniformBuffer createUniformBuffer(Context& context, uint64_t size)
    {
        const uint32_t frames{ static_cast<uint32_t>(context.screen.swapchainImages.size()) };
        uint64_t actualSize{ size * frames };
        UniformBuffer result{};
        result.perFrameOffset = size;

        constexpr VkMemoryPropertyFlags stagingPropertyFlags
        {
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };
        constexpr VkBufferUsageFlags stagingUsageFlags
        {
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        };
        result.stagingBuffer = createAllocatedBuffer(context.device, actualSize, stagingPropertyFlags, stagingUsageFlags);
        YGGDRASIL_ASSERT_VALUE(result.stagingBuffer.size == actualSize);
        YGGDRASIL_ASSERT_VALUE(result.stagingBuffer.buffer);
        YGGDRASIL_ASSERT_VALUE(result.stagingBuffer.memory);

        constexpr VkMemoryPropertyFlags uboPropertyFlags
        {
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        };
        constexpr VkBufferUsageFlags uboUsageFlags
        {
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
            VK_BUFFER_USAGE_TRANSFER_DST_BIT
        };
        result.ubo = createAllocatedBuffer(context.device, actualSize, uboPropertyFlags, uboUsageFlags);
        YGGDRASIL_ASSERT_VALUE(result.ubo.size == actualSize);
        YGGDRASIL_ASSERT_VALUE(result.ubo.buffer);
        YGGDRASIL_ASSERT_VALUE(result.ubo.memory);

        return result;
    }

    void uploadDataToUniformBuffer(void* data, uint64_t size, UniformBuffer& ubo, uint32_t currentFrame, VkCommandBuffer commandBuffer)
    {
        uint64_t offset{ ubo.perFrameOffset * currentFrame };
        void* dst{ &static_cast<uint8_t*>(ubo.stagingBuffer.data)[offset] };
        memcpy(dst, data, size);

        VkBufferCopy region{};
        region.srcOffset = offset;
        region.dstOffset = offset;
        region.size = size;
        vkCmdCopyBuffer(commandBuffer, ubo.stagingBuffer.buffer, ubo.ubo.buffer, 1, &region);

        // TODO: this should be batched!
        VkBufferMemoryBarrier barrier{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.offset = 0;
        barrier.size = VK_WHOLE_SIZE;
        barrier.buffer = ubo.ubo.buffer;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_DEPENDENCY_BY_REGION_BIT,
            0, nullptr,
            1, &barrier,
            0, nullptr);
    }

    void destroyUniformBuffer(Context& context, UniformBuffer& uniformBuffer)
    {
        YGGDRASIL_CORE_TRACE("Destroying Uniform Buffer [device].");
        destroyAllocatedBuffer(context.device, uniformBuffer.ubo);
        YGGDRASIL_CORE_TRACE("Destroying Uniform Buffer [staging].");
        destroyAllocatedBuffer(context.device, uniformBuffer.stagingBuffer);
    }
}
