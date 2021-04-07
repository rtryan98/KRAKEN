#pragma once
#include "Yggdrasil/Common/Window/Window.h"

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
        Game* pGame{};
    };

    void Init(const InitInfo* pInitInfo);
    void Run();
    void Shutdown();
}
