#pragma once
#include <vector>

namespace Ygg
{
    class GraphicsContext;

    class RenderEngine
    {
    public:
        RenderEngine();

        static RenderEngine& Get();
        static void Init();
        static void Render();
        static void Shutdown();

    private:
        GraphicsContext* context{};

        struct PerFrame
        {

        };
        std::vector<PerFrame> frames{};

        static RenderEngine* instance;
    };
}
