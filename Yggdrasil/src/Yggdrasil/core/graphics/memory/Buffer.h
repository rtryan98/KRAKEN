#pragma once
#include <vulkan/vulkan.h>

namespace yggdrasil::graphics
{
    class Renderer;
    class Device;
}

namespace yggdrasil::graphics::memory
{
    enum BufferType : uint32_t
    {
        BUFFER_TYPE_VERTEX   = 1,
        BUFFER_TYPE_INDEX    = 2,
        BUFFER_TYPE_INDIRECT = 4,
        BUFFER_TYPE_STORAGE  = 8,
        BUFFER_TYPE_UNIFORM  = 16,
        BUFFER_TYPE_STAGING  = 32
    };

    enum BufferUsage : uint32_t
    {
        BUFFER_USAGE_UPDATE_EVERY_FRAME = 1
    };

    class Buffer
    {
    private:
        VkBuffer       handle{};
        VkDeviceMemory memory{};
        uint32_t       type{};
        uint32_t       usage{};
        uint64_t       size{};
        void*          data{};

    private:
        void create(const Renderer* const renderer, uint32_t bufferType, uint32_t bufferUsage, uint64_t bufferSize);
        void bindMemory(const Device& device);
        void map(const Device& device);
        void unmap(const Device& device);
        void flush(const Device& device);
        void invalidate(const Device& device);
        void destroy(const Device& device);
        void copy(Buffer* target, uint64_t srcOffset, uint64_t dstOffset, uint64_t copySize, VkCommandBuffer commandBuffer);
        void upload(const Renderer* const renderer, void* bufferData, uint64_t dataSize, uint64_t bufferOffset);

    private:
        friend class yggdrasil::graphics::Renderer;
    };

    struct BufferCopy
    {
        Buffer* src{};
        Buffer* dst{};
    };
}
