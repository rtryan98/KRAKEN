#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"

namespace Ygg
{
    void RenderEngine::Init()
    {
        this->context.Create();
        this->frames.resize(this->context.GetScreen().GetData().swapchainImageCount);
    }

    void RenderEngine::Render()
    {

    }

    void RenderEngine::Shutdown()
    {
        this->context.Destroy();
    }

    GraphicsContext& RenderEngine::GetGraphicsContext()
    {
        return this->context;
    }
}
