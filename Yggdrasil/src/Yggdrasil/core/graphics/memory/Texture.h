#pragma once
#include "Yggdrasil/Types.h"
#include "Yggdrasil/core/graphics/memory/MemoryEnums.h"
#include <vulkan/vulkan.h>

namespace ygg::graphics
{
    class GraphicsEngine;
    class TextureManager;
    class Device;
}

namespace ygg::graphics::memory
{
    class Texture
    {
    private:
        VkImage               handle{};
        VkDeviceMemory        memory{};
        VkImageView           view{};
        VkDescriptorImageInfo descriptor{};
        VkSampler             sampler{};
        VkFormat              format{};
        TextureType           type{};
        uint64_t              size{};
        uint32_t              width{};
        uint32_t              height{};
        uint32_t              depth{};
        uint32_t              layers{};
        uint32_t              mipLevels{};
        VkImageLayout         currentLayout{};
        void*                 data{};

    private:
        void create(const GraphicsEngine* const graphicsEngine, TextureType textureType,
            uint32_t width, uint32_t height, uint32_t depth, uint32_t layers,
            VkFormat textureFormat, TextureTiling textureTiling = TextureTiling::TEXTURE_TILING_OPTIMAL,
            bool_t createTextureSampler = true);
        void destroy(const Device& device);
        void createView(const GraphicsEngine* const graphicsEngine);
        void createSampler(const GraphicsEngine* const graphicsEngine);
        void allocate(const GraphicsEngine* const graphicsEngine);
        VkImageMemoryBarrier getLayoutTransitionBarrier(VkImageLayout dst);
    private:
        friend class ygg::graphics::GraphicsEngine;
        friend class Buffer;
        friend class ygg::graphics::TextureManager;
    };
}
