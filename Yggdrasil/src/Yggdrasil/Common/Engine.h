// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#include "Yggdrasil/Common/Window/Window.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"

#include <vector>

namespace Ygg
{
    struct IGame;
    struct SInitInfo;

    class CEngine
    {
    public:
        CEngine();

        static bool IsRunning();
        static const CWindow& GetWindow();
        static CWindow& GetWindowNonConst();
        static void Init(const SInitInfo& initInfo);
        static void Update();
        static void Shutdown();

    private:
        static CEngine* s_instance;

        CRenderEngine m_renderEngine;
        CWindow m_window{};
        bool m_isRunning{ false };
        IGame* m_game{ nullptr };
    };
}
