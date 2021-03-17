#pragma once
#include "Yggdrasil/core/memory/Pool.h"
#include "Yggdrasil/core/graphics/memory/Texture.h"

namespace yggdrasil::graphics
{
    class TextureManager
    {
    public:
        void handleStagedTextures();

    private:
        ::yggdrasil::memory::Pool<memory::Texture, 8192> texturePool{};
    };
}
