#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/resources/ResourceManager.h"

namespace yggdrasil::graphics
{
    void ResourceManager::handleStagedResources(const GraphicsEngine* const graphicsEngine)
    {
        this->bufferManager.handleStagedBuffers(graphicsEngine);
    }
}
