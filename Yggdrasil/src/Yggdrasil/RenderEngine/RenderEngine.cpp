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
        this->context = new GraphicsContext();
        this->context->Create();
        this->frames.resize(this->context->GetScreen().GetData().swapchainImageCount);
    }

    void RenderEngine::Render()
    {

    }

    void RenderEngine::Shutdown()
    {
        this->context->Destroy();
        delete this->context;
    }

    RenderEngine& RenderEngine::Get()
    {
        return *RenderEngine::instance;
    }
}
