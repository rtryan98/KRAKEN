#pragma once
#include <vulkan/vulkan.h>

namespace yggdrasil::graphics
{
    class Renderer;
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
        TEXTURE_TYPE_CUBEMAP,
        TEXTURE_TYPE_CUBEMAP_ARRAY
    };

    enum TextureFormat : uint32_t
    {

    };

    class Texture
    {
    private:
        VkImage               handle{};
        VkDeviceMemory        memory{};
        VkImageView           view{};
        VkDescriptorImageInfo descriptor{};
        VkSampler             sampler{};
        TextureType           type{};
        uint32_t              width{};
        uint32_t              height{};
        uint32_t              depth{};
        uint32_t              layers{};
        uint32_t              mipLevels{};
        void*                 data{};

    private:
        void create(const Renderer* const renderer, TextureType textureType);
        void destroy(const Device& device);
        void createSampler(const Renderer* const renderer, TextureType textureType);
    private:
        friend class yggdrasil::graphics::Renderer;
    };
}
