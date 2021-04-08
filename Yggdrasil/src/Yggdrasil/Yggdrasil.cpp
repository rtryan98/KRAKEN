#include "Yggdrasil/pch.h"
#include "Yggdrasil/Yggdrasil.h"
#include "Yggdrasil/Common/Engine.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"
#include "Yggdrasil/Common/Util/Logger.h"

namespace Ygg
{
    Engine* Engine::instance = nullptr;

    RenderEngine* renderEngine{ nullptr };

    void Init(const InitInfo* initInfo)
    {
        if (initInfo->pGame == nullptr)
        {
            std::terminate();
        }
        Logger::Init();
        Engine::instance = new Engine();
        Engine::instance->window.Create(&initInfo->windowCreateInfo);
        renderEngine = new RenderEngine();
        RenderEngine::Init();
        Engine::instance->game = initInfo->pGame;
        Engine::instance->game->Init();
    }

    void Run()
    {
        Engine::instance->isRunning = true;
        while (Engine::instance->isRunning)
        {
            Engine::instance->window.Update();
            Engine::instance->game->Update();
            RenderEngine::Render();
            if (Engine::instance->window.IsClosed())
            {
                Engine::instance->isRunning = false;
            }
        }
        Shutdown();
    }

    void Shutdown()
    {
        RenderEngine::Shutdown();
        delete renderEngine;
        Engine::instance->game->Shutdown();
        delete Engine::instance;
        Logger::Shutdown();
    }
}
