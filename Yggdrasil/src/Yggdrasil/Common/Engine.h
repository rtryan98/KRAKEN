#pragma once
#include "Yggdrasil/Common/Window/Window.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"

#include <vector>

namespace Ygg
{
    class Game;
    struct InitInfo;

    class Engine
    {
    public:
        Engine();

        static bool IsRunning();
        static Window& GetWindow();
        static void Init(const InitInfo& initInfo);
        static void Update();
        static void ShutDown();

    private:
        static Engine* instance;

        RenderEngine renderEngine;
        Window window{};
        bool isRunning{ false };
        Game* game{ nullptr };
    };
}
