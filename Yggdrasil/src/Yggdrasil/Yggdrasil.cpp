// Copyright 2021 Robert Ryan. See LICENCE.md.

#include "Yggdrasil/pch.h"
#include "Yggdrasil/Yggdrasil.h"
#include "Yggdrasil/Common/Engine.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"
#include "Yggdrasil/Common/Util/Logger.h"

namespace Ygg
{
    CEngine* engine{ nullptr };

    void Init(const SInitInfo& initInfo)
    {
        if (initInfo.pGame == nullptr)
        {
            std::terminate();
        }
        engine = new CEngine();
        CEngine::Init(initInfo);
    }

    void Run()
    {
        while (CEngine::IsRunning())
        {
            CEngine::Update();
        }
        Shutdown();
    }

    void Shutdown()
    {
        CEngine::Shutdown();
        delete engine;
    }
}
