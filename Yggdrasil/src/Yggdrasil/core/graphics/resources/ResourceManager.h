#pragma once
#include "Yggdrasil/core/graphics/resources/BufferManager.h"
#include "Yggdrasil/core/graphics/resources/TextureManager.h"

namespace yggdrasil::graphics
{
    class ResourceManager
    {
    public:
        void handleStagedResources(const GraphicsEngine* const graphicsEngine);

        BufferManager bufferManager{};
        TextureManager textureManager{};
    };
}
