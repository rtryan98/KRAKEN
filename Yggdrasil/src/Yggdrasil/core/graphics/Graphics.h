#pragma once
#include "Yggdrasil/core/graphics/memory/MemoryEnums.h"

namespace ygg::graphics
{
    namespace memory
    {
        class Buffer;
        class Texture;
    }

    memory::Buffer*  createBuffer(uint32_t size, uint32_t type, uint32_t usage = 0);
    void             uploadBufferData(memory::Buffer* buffer, void* data, uint32_t size, uint32_t offset = 0);
    void             destroyBuffer(memory::Buffer* buffer);

    memory::Texture* createTexture2DFromFile(const char* fileName);
    void             destroyTexture(memory::Texture* texture);
}
