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
        BUFFER_TYPE_UNIFORM  = 16
    };

    enum BufferUsage : uint32_t
    {
        BUFFER_USAGE_DEFAULT = 0,
        BUFFER_USAGE_UPDATE_EVERY_FRAME = 1
    };

    class Buffer
    {
    public:
        VkBuffer       handle{};

    private:
        VkDeviceMemory memory{};
        BufferType     type{};
        BufferUsage    usage{};
        void*          data{};

    private:
        void create(const Renderer* const renderer, BufferType bufferType, BufferUsage bufferUsage, uint64_t bufferSize);
        void bindMemory(const Device& device);
        void* map(const Device& device);
        void unmap(const Device& device);
        void flush(const Device& device);
        void invalidate(const Device& device);
        void destroy(const Device& device);

    private:
        friend class yggdrasil::graphics::Renderer;
    };
}
