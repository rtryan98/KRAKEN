// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#include "Yggdrasil/RenderEngine/GraphicsDevice.h"
#include "Yggdrasil/RenderEngine/Screen.h"

#include <vulkan/vulkan.h>

namespace Ygg
{
    class CGraphicsContext
    {
    public:
        void Create();
        void Destroy();
        const CScreen& GetScreen() const;
        VkInstance GetVkInstance() const;
        const CGraphicsDevice& GetGraphicsDevice() const;
        CGraphicsDevice& GetGraphicsDeviceNonConst();

    private:
        VkInstance m_instance{};
        CGraphicsDevice m_device{};
        CScreen m_screen{};
    };
}
