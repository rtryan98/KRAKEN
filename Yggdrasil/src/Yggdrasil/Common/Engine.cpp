#include "Yggdrasil/pch.h"
#include "Yggdrasil/Common/Engine.h"
#include "Yggdrasil/Yggdrasil.h"

namespace Ygg
{
    Engine* Engine::instance = nullptr;

    Engine::Engine()
    {
        Engine::instance = this;
    }

    bool Engine::IsRunning()
    {
        return Engine::instance->isRunning;
    }

    Window& Engine::GetWindow()
    {
        return Engine::instance->window;
    }

    void Engine::Init(const InitInfo& initInfo)
    {
        Logger::Init();
        Engine::instance->window.Create(initInfo.windowCreateInfo);
        Engine::instance->renderEngine.Init();
        Engine::instance->game = initInfo.pGame;
        Engine::instance->game->Init();
        Engine::instance->isRunning = true;
    }

    void Engine::Update()
    {
        Engine::instance->window.Update();
        Engine::instance->game->Update();
        Engine::instance->renderEngine.Render();
        if (Engine::instance->window.IsClosed())
        {
            Engine::instance->isRunning = false;
        }
    }

    void Engine::ShutDown()
    {
        Engine::instance->renderEngine.Shutdown();
        Engine::instance->game->Shutdown();
        Logger::Shutdown();
    }
}
