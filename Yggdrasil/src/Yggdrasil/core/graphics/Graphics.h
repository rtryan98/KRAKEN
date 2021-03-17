#pragma once
#include "Yggdrasil/core/graphics/memory/MemoryEnums.h"

namespace yggdrasil::graphics
{
    namespace memory
    {
        class Buffer;
        class Texture;
    }

    inline memory::Buffer*  createBuffer(uint32_t size, uint32_t type, uint32_t usage = 0);
    inline void             uploadBufferData(memory::Buffer* buffer, void* data, uint32_t size, uint32_t offset = 0);
    inline void             destroyBuffer(memory::Buffer* buffer);

    inline memory::Texture* createTexture2DFromFile(const char* fileName);
    inline void             destroyTexture(memory::Texture* texture);
}
