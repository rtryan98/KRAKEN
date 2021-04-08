#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"

namespace Ygg
{
    RenderEngine* RenderEngine::instance = nullptr;

    RenderEngine::RenderEngine()
    {
        RenderEngine::instance = this;
    }

    void RenderEngine::Init()
    {
        RenderEngine::instance->context = new GraphicsContext();
        RenderEngine::instance->context->Create();
        RenderEngine::instance->frames.resize(RenderEngine::instance->context->GetScreen().GetData().swapchainImageCount);
    }

    void RenderEngine::Render()
    {

    }

    void RenderEngine::Shutdown()
    {
        RenderEngine::instance->context->Destroy();
        delete RenderEngine::instance->context;
    }

    RenderEngine& RenderEngine::Get()
    {
        return *RenderEngine::instance;
    }
}
