#pragma once
#include "Yggdrasil/core/memory/Pool.h"
#include "Yggdrasil/core/graphics/memory/Texture.h"
#include "Yggdrasil/core/graphics/memory/Buffer.h"

#include <vector>

namespace ygg::graphics
{
    class TextureManager
    {
    public:
        void create(GraphicsEngine* const graphicsEngine);
        void destroy(GraphicsEngine* const graphicsEngine);
        memory::Texture* createTexture(const GraphicsEngine* const graphicsEngine, memory::TextureType textureType,
            uint32_t width, uint32_t height, uint32_t depth, uint32_t layers,
            VkFormat textureFormat, memory::TextureTiling textureTiling = memory::TextureTiling::TEXTURE_TILING_OPTIMAL,
            bool_t createTextureSampler = true);
        memory::Texture* createTexture2DFromFile(GraphicsEngine* const graphicsEngine, const char* fileName);
        void destroyTexture(const GraphicsEngine* const graphicsEngine, memory::Texture* target);
        void handleStagedTextures(GraphicsEngine* const graphicsEngine);
        void uploadTexture(GraphicsEngine* const graphicsEngine, memory::Texture* texture,
            void* textureData, uint32_t textureSize);

    private:
        ::ygg::memory::Pool<memory::Texture, 8192> texturePool{};
        std::vector<memory::Buffer*> stagingBuffers{};
        std::vector<memory::BufferToTextureCopy> bufferToTextureCopies{};
        uint32_t currentStagingOffset{ 0 };
    };
}
