#include "Yggdrasil/pch.h"
#include "Yggdrasil/Yggdrasil.h"
#include "Yggdrasil/Common/Engine.h"

namespace Ygg
{
    Engine* Engine::instance = nullptr;

    void Init(const InitInfo* initInfo)
    {
        Engine::instance = new Engine();
        Engine::instance->window.Create(&initInfo->windowCreateInfo);
        Engine::instance->updateFunctions = initInfo->functionInfo.updateFunctions;
        Engine::instance->shutdownFunctions = initInfo->functionInfo.shutdownFunctions;
        for (auto& fn : initInfo->functionInfo.initFunctions)
        {
            fn();
        }
    }

    void Run()
    {
        Engine::instance->isRunning = true;
        while (Engine::instance->isRunning)
        {
            Engine::instance->window.Update();
            for (auto& fn : Engine::instance->updateFunctions)
            {
                fn();
            }
            if (Engine::instance->window.IsClosed())
            {
                Engine::instance->isRunning = false;
            }
        }
    }

    void Shutdown()
    {
        for (auto& fn : Engine::instance->shutdownFunctions)
        {
            fn();
        }
        Engine::instance->window.Destroy();
        delete Engine::instance;
    }
}
