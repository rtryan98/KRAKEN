#pragma once
#include "Yggdrasil/core/graphics/resources/BufferManager.h"
#include "Yggdrasil/core/graphics/resources/TextureManager.h"

namespace ygg::graphics
{
    class ResourceManager
    {
    public:
        void handleStagedResources(GraphicsEngine* const graphicsEngine);
        void create(GraphicsEngine* const graphicsEngine);
        void destroy(GraphicsEngine* const graphicsEngine);

        BufferManager bufferManager{};
        TextureManager textureManager{};
    };
}
