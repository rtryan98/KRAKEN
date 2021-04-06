#include "Yggdrasil/pch.h"
#include "Yggdrasil/Yggdrasil.h"
#include "Yggdrasil/Common/Engine.h"

namespace Ygg
{
    Engine* Engine::instance = nullptr;

    void Init(const InitInfo* initInfo)
    {
        if (initInfo->game == nullptr)
        {
            std::terminate();
        }
        Engine::instance = new Engine();
        Engine::instance->window.Create(&initInfo->windowCreateInfo);
        Engine::instance->game = initInfo->game;
        Engine::instance->game->Init();
    }

    void Run()
    {
        Engine::instance->isRunning = true;
        while (Engine::instance->isRunning)
        {
            Engine::instance->window.Update();
            Engine::instance->game->Update();
            if (Engine::instance->window.IsClosed())
            {
                Engine::instance->isRunning = false;
            }
        }
    }

    void Shutdown()
    {
        Engine::instance->game->Shutdown();
        delete Engine::instance;
    }
}
