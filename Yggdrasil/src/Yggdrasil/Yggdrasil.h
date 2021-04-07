#pragma once
#include "Yggdrasil/Common/Window/Window.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"

#include <vector>

namespace Ygg
{
    class Game
    {
    public:
        virtual void Init();
        virtual void Update();
        virtual void Shutdown();
    };

    struct InitInfo
    {
        WindowCreateInfo windowCreateInfo{};
        RenderEngineFeatures* pRenderEngineFeatures{ nullptr };
        Game* pGame{};
    };

    void Init(const InitInfo* pInitInfo);
    void Run();
    void Shutdown();
}
