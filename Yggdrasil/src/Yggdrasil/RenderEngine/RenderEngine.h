#pragma once
#include "Yggdrasil/RenderEngine/GraphicsContext.h"

#include <vector>

namespace Ygg
{
    class CRenderEngine
    {
    public:
        void Init();
        void Render();
        void Shutdown();
        const CGraphicsContext& GetGraphicsContext() const;

    private:
        CGraphicsContext m_context{};

        struct SPerFrame
        {

        };
        std::vector<SPerFrame> m_frames{};
    };
}
