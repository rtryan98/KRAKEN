#pragma once
#include "Yggdrasil/core/memory/Pool.h"
#include "Yggdrasil/core/graphics/memory/Buffer.h"

#include <vector>

namespace yggdrasil::graphics
{
    class Device;
    class GraphicsEngine;

    class BufferManager
    {
    public:
        void create(const GraphicsEngine* const graphicsEngine);
        void destroy(const GraphicsEngine* const graphicsEngine);
        void handleStagedBuffers(const GraphicsEngine* const graphicsEngine);
        memory::Buffer* createBuffer(const GraphicsEngine* const graphicsEngine, uint32_t bufferType, uint32_t bufferUsage, uint64_t bufferSize);
        void uploadDataToBuffer(const GraphicsEngine* const graphicsEngine, memory::Buffer* target, void* bufferData, uint64_t dataSize, uint64_t bufferOffset = 0);
        void destroyBuffer(const GraphicsEngine* const graphicsEngine, memory::Buffer* buffer);
    private:
        ::yggdrasil::memory::Pool<memory::Buffer, 8192> bufferPool{};
        std::vector<memory::BufferCopy> stagedBufferCopies{};
        std::vector<memory::Buffer*> stagingBuffers{};
        uint64_t currentStagingOffset{ 0 };
    };
}
