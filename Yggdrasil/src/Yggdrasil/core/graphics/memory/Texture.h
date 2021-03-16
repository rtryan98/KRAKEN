#pragma once
#include <vulkan/vulkan.h>

namespace yggdrasil::graphics
{
    class GraphicsEngine;
    class Device;
}

namespace yggdrasil::graphics::memory
{
    enum TextureType : uint32_t
    {
        TEXTURE_TYPE_1D = 0,
        TEXTURE_TYPE_1D_ARRAY,
        TEXTURE_TYPE_2D,
        TEXTURE_TYPE_2D_ARRAY,
        TEXTURE_TYPE_3D,
        TEXTURE_TYPE_3D_ARRAY,
        TEXTURE_TYPE_CUBEMAP
    };

    enum class TextureTiling : uint32_t
    {
        TEXTURE_TILING_OPTIMAL,
        TEXTURE_TILING_LINEAR
    };

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
        uint32_t              width{};
        uint32_t              height{};
        uint32_t              depth{};
        uint32_t              layers{};
        uint32_t              mipLevels{};
        void*                 data{};

    private:
        void create(const GraphicsEngine* const graphicsEngine, TextureType textureType,
            uint32_t width, uint32_t height, uint32_t depth, uint32_t layers,
            VkFormat textureFormat, TextureTiling textureTiling = TextureTiling::TEXTURE_TILING_OPTIMAL,
            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED);
        void destroy(const Device& device);
        void createView(const GraphicsEngine* const graphicsEngine);
        void createSampler(const GraphicsEngine* const graphicsEngine);
        void allocate(const GraphicsEngine* const graphicsEngine);
        VkImageMemoryBarrier getLayoutTransitionBarrier(VkImageLayout dst);
    private:
        friend class yggdrasil::graphics::GraphicsEngine;
        friend class Buffer;
    };
}
