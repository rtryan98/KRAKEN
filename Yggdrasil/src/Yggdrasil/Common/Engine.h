#pragma once
#include "Yggdrasil/Common/Window/Window.h"

#include <vector>

namespace Ygg
{
    class Game;

    struct Engine
    {
        static Engine* instance;
        Window window;
        bool isRunning{ false };
        Game* game;
    };
}
