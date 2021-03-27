#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/Graphics.h"
#include "Yggdrasil/core/graphics/GraphicsEngine.h"

namespace ygg::graphics
{
    memory::Buffer* createBuffer(uint32_t size, uint32_t type, uint32_t usage)
    {
        return GraphicsEngine::get()->getResourceManager()
            .bufferManager.createBuffer(GraphicsEngine::get(), type, usage, size);
    }

    void uploadBufferData(memory::Buffer* buffer, void* data, uint32_t size, uint32_t offset)
    {
        GraphicsEngine::get()->getResourceManager()
            .bufferManager.uploadDataToBuffer(GraphicsEngine::get(), buffer, data, size, offset);
    }

    void destroyBuffer(memory::Buffer* buffer)
    {
        GraphicsEngine::get()->getResourceManager()
            .bufferManager.destroyBuffer(GraphicsEngine::get(), buffer);
    }

    memory::Texture* createTexture2DFromFile(const char* fileName)
    {
        return GraphicsEngine::get()->getResourceManager()
            .textureManager.createTexture2DFromFile(GraphicsEngine::get(), fileName);
    }

    void destroyTexture(memory::Texture* texture)
    {
        GraphicsEngine::get()->getResourceManager()
            .textureManager.destroyTexture(GraphicsEngine::get(), texture);
    }
}
