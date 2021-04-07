#pragma once

namespace Ygg
{
    struct GraphicsContext;

    struct RenderEngineFeatures
    {
        bool enableAllFeatures{ false };
    };

    struct RenderEngine
    {
        static RenderEngine* instance;
        GraphicsContext* context;

        void Init(RenderEngineFeatures* features);
        void Shutdown();
    };
}
