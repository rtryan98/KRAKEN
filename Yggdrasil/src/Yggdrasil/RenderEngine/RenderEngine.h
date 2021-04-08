#pragma once
#include "Yggdrasil/RenderEngine/GraphicsContext.h"

#include <vector>

namespace Ygg
{
    class RenderEngine
    {
    public:
        void Init();
        void Render();
        void Shutdown();
        GraphicsContext& GetGraphicsContext();

    private:
        GraphicsContext context{};

        struct PerFrame
        {

        };
        std::vector<PerFrame> frames{};
    };
}
