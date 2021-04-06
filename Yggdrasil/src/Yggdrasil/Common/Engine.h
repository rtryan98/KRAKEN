#pragma once
#include "Yggdrasil/Common/Window/Window.h"

#include <vector>

namespace Ygg
{
    struct Engine
    {
        static Engine* instance;
        Window window;
        bool isRunning{ false };

        std::vector<void(*)()> updateFunctions;
        std::vector<void(*)()> shutdownFunctions;
    };
}
