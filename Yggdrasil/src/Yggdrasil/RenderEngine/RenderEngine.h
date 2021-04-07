#pragma once
#include <vector>

namespace Ygg
{
    struct GraphicsContext;

    struct RenderEngine
    {
        static RenderEngine* instance;
        GraphicsContext* context;

        struct PerFrame
        {

        };

        std::vector<PerFrame> frames{};

        void Init();
        void Render();
        void Shutdown();
    };
}
