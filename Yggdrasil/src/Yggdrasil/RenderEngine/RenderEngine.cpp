#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"

namespace Ygg
{
    void RenderEngine::Init(RenderEngineFeatures* features)
    {
        this->context = new GraphicsContext();
        this->context->Create(features);
    }

    void RenderEngine::Shutdown()
    {
        this->context->Destroy();
        delete this->context;
    }
}
