#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/memory/Texture.h"
#include "Yggdrasil/core/graphics/Util.h"
#include "Yggdrasil/core/graphics/Globals.h"
#include "Yggdrasil/core/graphics/GraphicsEngine.h"
#include "Yggdrasil/core/graphics/memory/Allocator.h"

namespace yggdrasil::graphics::memory
{
    void Texture::create(const GraphicsEngine* const graphicsEngine, TextureType textureType,
        uint32_t textureWidth, uint32_t textureHeight, uint32_t textureDepth, uint32_t textureLayers,
        VkFormat textureFormat, TextureTiling textureTiling, VkImageLayout initialLayout)
    {
        this->width = textureWidth;
        this->height = textureHeight;
        this->depth = textureDepth;
        this->layers = textureLayers;
        this->format = textureFormat;
        this->type = textureType;
        this->mipLevels = 1;

        VkImageCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        createInfo.arrayLayers = textureLayers;
        createInfo.mipLevels = 1;
        createInfo.format = textureFormat;

        switch (textureTiling)
        {
            case TextureTiling::TEXTURE_TILING_OPTIMAL:
                createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                break;
            case TextureTiling::TEXTURE_TILING_LINEAR:
                createInfo.tiling = VK_IMAGE_TILING_LINEAR;
                break;
        }

        switch (textureType)
        {
            case TEXTURE_TYPE_1D_ARRAY:
                [[fallthrough]];
            case TEXTURE_TYPE_1D:
                createInfo.imageType = VK_IMAGE_TYPE_1D;
                break;
            case TEXTURE_TYPE_2D_ARRAY:
                createInfo.flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
                [[fallthrough]];
            case TEXTURE_TYPE_2D:
                createInfo.imageType = VK_IMAGE_TYPE_2D;
                break;
            case TEXTURE_TYPE_3D_ARRAY:
                [[fallthrough]];
            case TEXTURE_TYPE_3D            :
                createInfo.imageType = VK_IMAGE_TYPE_3D;
                break;
            case TEXTURE_TYPE_CUBEMAP:
                createInfo.imageType = VK_IMAGE_TYPE_2D;
                createInfo.arrayLayers *= 6;
                this->layers *= 6;
                createInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                break;
        }

        // TODO: set this to exclusive
        createInfo.queueFamilyIndexCount = 1;
        createInfo.pQueueFamilyIndices = &graphicsEngine->getContext().device.queues.rasterizerQueueFamilyIndex;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        createInfo.initialLayout = initialLayout;
        createInfo.extent.width = textureWidth;
        createInfo.extent.height = textureHeight;
        createInfo.extent.depth = textureDepth;

        VK_CHECK( vkCreateImage(graphicsEngine->getContext().device.logical, &createInfo, VK_CPU_ALLOCATOR, &this->handle) );

        allocate(graphicsEngine);
        createView(graphicsEngine);
        createSampler(graphicsEngine);
    }

    void Texture::allocate(const GraphicsEngine* const graphicsEngine)
    {
        VkMemoryRequirements memoryRequirements{};
        vkGetImageMemoryRequirements(graphicsEngine->getContext().device.logical, this->handle, &memoryRequirements);

        VkMemoryAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocateInfo.allocationSize = memoryRequirements.size;
        allocateInfo.memoryTypeIndex = getMemoryTypeIndex(graphicsEngine->getContext().device,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        VK_CHECK( vkAllocateMemory(graphicsEngine->getContext().device.logical, &allocateInfo, VK_CPU_ALLOCATOR, &this->memory) );
        VK_CHECK( vkBindImageMemory(graphicsEngine->getContext().device.logical, this->handle, this->memory, 0) );
    }

    void Texture::createView(const GraphicsEngine* const graphicsEngine)
    {
        VkImageViewCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        createInfo.image = this->handle;
        graphicsEngine;
    }

    void Texture::createSampler(const GraphicsEngine* const graphicsEngine)
    {
        VkSamplerCreateInfo createInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        graphicsEngine;
    }

    VkImageMemoryBarrier Texture::getLayoutTransitionBarrier(VkImageLayout dst)
    {
        VkImageMemoryBarrier result{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        result.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        result.newLayout = dst;
        result.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        result.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        result.image = this->handle;
        result.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        result.subresourceRange.layerCount = this->layers;
        result.subresourceRange.baseArrayLayer = 0;
        result.subresourceRange.levelCount = this->mipLevels;
        result.subresourceRange.baseMipLevel = 0;

        return result;
    }

    void Texture::destroy(const Device& device)
    {
        util::destroy(&this->view, vkDestroyImageView, device.logical);
        util::destroy(&this->handle, vkDestroyImage, device.logical);
        util::destroy(&this->sampler, vkDestroySampler, device.logical);
        if (this->memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device.logical, this->memory, VK_CPU_ALLOCATOR);
        }
    }
}
