#pragma once
#include "Yggdrasil/RenderEngine/GraphicsDevice.h"
#include "Yggdrasil/RenderEngine/Screen.h"

#include <vulkan/vulkan.h>

namespace Ygg
{
    class GraphicsContext
    {
    public:
        void Create();
        void Destroy();
        Screen& GetScreen();
        VkInstance GetVkInstance();
        GraphicsDevice* const GetGraphicsDevice();

    private:
        VkInstance instance{};
        GraphicsDevice* pDevice{};
        Screen screen{};
    };
}
