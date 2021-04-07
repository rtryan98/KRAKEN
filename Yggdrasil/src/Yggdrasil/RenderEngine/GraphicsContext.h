#pragma once
#include "Yggdrasil/RenderEngine/GraphicsDevice.h"
#include "Yggdrasil/RenderEngine/Screen.h"

#include <vulkan/vulkan.h>

namespace Ygg
{
    struct GraphicsContext
    {
        VkInstance instance{};
        GraphicsDevice* pDevice{};
        Screen screen{};

        void Create();
        void Destroy();
    };
}
