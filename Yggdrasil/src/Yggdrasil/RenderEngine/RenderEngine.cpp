#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"

namespace Ygg
{
    void RenderEngine::Init()
    {
        this->context = new GraphicsContext();
        this->context->Create();
        this->frames.resize(this->context->screen.swapchainImageCount);
    }

    void RenderEngine::Render()
    {

    }

    void RenderEngine::Shutdown()
    {
        this->context->Destroy();
        delete this->context;
    }
}
