#pragma once

namespace ygg::graphics::memory
{
    enum TextureType : uint32_t
    {
        TEXTURE_TYPE_1D = 0,
        TEXTURE_TYPE_1D_ARRAY,
        TEXTURE_TYPE_2D,
        TEXTURE_TYPE_2D_ARRAY,
        TEXTURE_TYPE_3D,
        TEXTURE_TYPE_CUBEMAP
    };

    enum class TextureTiling : uint32_t
    {
        TEXTURE_TILING_OPTIMAL,
        TEXTURE_TILING_LINEAR
    };

    enum BufferType : uint32_t
    {
        BUFFER_TYPE_VERTEX = 1,
        BUFFER_TYPE_INDEX = 2,
        BUFFER_TYPE_INDIRECT = 4,
        BUFFER_TYPE_STORAGE = 8,
        BUFFER_TYPE_UNIFORM = 16,
        BUFFER_TYPE_STAGING = 32
    };

    enum BufferUsage : uint32_t
    {
        BUFFER_USAGE_UPDATE_EVERY_FRAME = 1
    };
}
