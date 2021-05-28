// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#include "Yggdrasil/Common/Window/Window.h"

#include <vector>

namespace Ygg
{
    struct IGame
    {
        virtual void Init() {};
        virtual void Update() {};
        virtual void Shutdown() {};
    };

    struct SInitInfo
    {
        SWindowCreateInfo windowCreateInfo{};
        IGame* pGame{};
    };

    void Init(const SInitInfo& pInitInfo);
    void Run();
    void Shutdown();
}
