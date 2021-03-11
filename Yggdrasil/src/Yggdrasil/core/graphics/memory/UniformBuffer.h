#pragma once
#include "Yggdrasil/core/graphics/Context.h"
#include "Yggdrasil/core/graphics/memory/Resource.h"
#include <vulkan/vulkan.h>

namespace yggdrasil::graphics::memory
{
    struct UniformBuffer
    {
        AllocatedBuffer ubo{};
        AllocatedBuffer stagingBuffer{};
        uint64_t perFrameOffset{};
    };

    UniformBuffer createUniformBuffer(Context& context, uint64_t size);
    void uploadDataToUniformBuffer(void* data, uint64_t size, UniformBuffer& ubo, uint32_t currentFrame, VkCommandBuffer commandBuffer);
    void destroyUniformBuffer(Context& context, UniformBuffer& uniformBuffer);
}
