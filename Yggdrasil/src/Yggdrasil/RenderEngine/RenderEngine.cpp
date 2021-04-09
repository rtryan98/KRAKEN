#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"

namespace Ygg
{
    void CRenderEngine::Init()
    {
        this->m_context.Create();
        this->m_frames.resize(this->m_context.GetScreen().GetData().swapchainImageCount);
    }

    void CRenderEngine::Render()
    {

    }

    void CRenderEngine::Shutdown()
    {
        this->m_context.Destroy();
    }

    const CGraphicsContext& CRenderEngine::GetGraphicsContext() const
    {
        return this->m_context;
    }
}
