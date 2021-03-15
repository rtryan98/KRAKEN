#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/memory/Texture.h"
#include "Yggdrasil/core/graphics/Util.h"
#include "Yggdrasil/core/graphics/Globals.h"

namespace yggdrasil::graphics::memory
{
    void Texture::create(const Renderer* const renderer, TextureType textureType)
    {
        renderer;
        textureType;
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
