#include "Yggdrasil/pch.h"
#include "Yggdrasil/Yggdrasil.h"
#include "Yggdrasil/Common/Engine.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"
#include "Yggdrasil/Common/Util/Logger.h"

namespace Ygg
{
    Engine* engine{ nullptr };

    void Init(const InitInfo& initInfo)
    {
        if (initInfo.pGame == nullptr)
        {
            std::terminate();
        }
        engine = new Engine();
        Engine::Init(initInfo);
    }

    void Run()
    {
        while (Engine::IsRunning())
        {
            Engine::Update();
        }
    }

    void Shutdown()
    {
        Engine::ShutDown();
        delete engine;
    }
}
