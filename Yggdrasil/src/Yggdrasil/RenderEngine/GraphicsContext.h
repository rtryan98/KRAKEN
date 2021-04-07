#pragma once
#include "Yggdrasil/RenderEngine/GraphicsDevice.h"
#include "Yggdrasil/RenderEngine/Screen.h"

#include <vulkan/vulkan.h>

namespace Ygg
{
    struct RenderEngineFeatures;

    struct GraphicsContext
    {
        VkInstance instance;
        GraphicsDevice* pDevice;
        Screen screen;

        void Create(RenderEngineFeatures* pFeatures);
        void Destroy();
    };
}
