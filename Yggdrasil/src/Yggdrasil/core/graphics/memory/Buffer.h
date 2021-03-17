#pragma once
#include "Yggdrasil/Defines.h"
#include <vulkan/vulkan.h>

namespace yggdrasil::graphics
{
    class GraphicsEngine;
    class BufferManager;
    class TextureManager;
    class Device;
}

namespace yggdrasil::graphics::memory
{
    class Texture;

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
        uint64_t       alignment{};
        void*          data{};

    private:
        void create(const GraphicsEngine* const graphicsEngine, uint32_t bufferType, uint32_t bufferUsage, uint64_t bufferSize);
        void bindMemory(const Device& device);
        void map(const Device& device);
        void unmap(const Device& device);
        void flush(const Device& device);
        void invalidate(const Device& device);
        void destroy(const Device& device);
        void copy(Buffer* target, uint64_t srcOffset, uint64_t dstOffset, uint64_t copySize, VkCommandBuffer commandBuffer);
        void copy(Texture* target, uint64_t srcOffset, VkCommandBuffer commandBuffer,
            uint32_t dstOffsetX = 0, uint32_t dstOffsetY = 0, uint32_t dstOffsetZ = 0,
            uint32_t mipLevel = 0, uint32_t layerCount = 1, uint32_t baseLayer = 0);
        void upload(const GraphicsEngine* const graphicsEngine, void* bufferData, uint64_t dataSize, uint64_t bufferOffset);

    private:
        friend class yggdrasil::graphics::GraphicsEngine;
        friend class yggdrasil::graphics::BufferManager;
        friend class yggdrasil::graphics::TextureManager;
    };

    struct BufferCopy
    {
        Buffer* src{};
        Buffer* dst{};
        uint64_t srcOffset{};
        uint64_t dstOffset{};
        uint64_t size{};
    };

    struct BufferToTextureCopy
    {
        Buffer* src{};
        Texture* dst{};
        uint32_t srcOffset{};
        uint32_t dstOffsetX{};
        uint32_t dstOffsetY{};
        uint32_t dstOffsetZ{};
    };
}
