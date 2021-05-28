// Copyright 2021 Robert Ryan. See LICENCE.md.

#include "Yggdrasil/pch.h"
#include "Yggdrasil/Common/Engine.h"
#include "Yggdrasil/Yggdrasil.h"

namespace Ygg
{
    CEngine* CEngine::s_instance = nullptr;

    CEngine::CEngine()
    {
        CEngine::s_instance = this;
    }

    bool CEngine::IsRunning()
    {
        return CEngine::s_instance->m_isRunning;
    }

    const CWindow& CEngine::GetWindow()
    {
        return CEngine::s_instance->m_window;
    }

    CWindow& CEngine::GetWindowNonConst()
    {
        return CEngine::s_instance->m_window;
    }

    void CEngine::Init(const SInitInfo& initInfo)
    {
        CLogger::Init();
        CEngine::s_instance->m_window.Create(initInfo.windowCreateInfo);
        CEngine::s_instance->m_renderEngine.Init();
        CEngine::s_instance->m_game = initInfo.pGame;
        CEngine::s_instance->m_game->Init();
        CEngine::s_instance->m_isRunning = true;
    }

    void CEngine::Update()
    {
        CEngine::s_instance->m_window.Update();
        CEngine::s_instance->m_game->Update();
        CEngine::s_instance->m_renderEngine.Render();
        if (CEngine::s_instance->m_window.IsClosed())
        {
            CEngine::s_instance->m_isRunning = false;
        }
    }

    void CEngine::Shutdown()
    {
        CEngine::s_instance->m_renderEngine.Shutdown();
        CEngine::s_instance->m_game->Shutdown();
        CLogger::Shutdown();
    }
}
