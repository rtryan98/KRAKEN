#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/resources/ResourceManager.h"
#include "Yggdrasil/core/graphics/GraphicsEngine.h"

namespace ygg::graphics
{
    void ResourceManager::create(GraphicsEngine* const graphicsEngine)
    {
        this->bufferManager.create(graphicsEngine);
        this->textureManager.create(graphicsEngine);
    }

    void ResourceManager::destroy(GraphicsEngine* const graphicsEngine)
    {
        this->textureManager.destroy(graphicsEngine);
        this->bufferManager.destroy(graphicsEngine);
    }

    void ResourceManager::handleStagedResources(GraphicsEngine* const graphicsEngine)
    {
        this->textureManager.handleStagedTextures(graphicsEngine);
        this->bufferManager.handleStagedBuffers(graphicsEngine);
    }
}
