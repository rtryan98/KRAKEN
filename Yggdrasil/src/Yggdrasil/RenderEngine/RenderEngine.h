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
        void InitPerFrameStruct();

    private:
        CGraphicsContext m_context{};

        struct SPerFrame
        {
            VkCommandPool cmdPool;
            VkCommandBuffer cmdBuffer;
            VkFence submitFence;
            VkSemaphore acquireSemaphore;
            VkSemaphore releaseSemaphore;
        };
        std::vector<SPerFrame> m_frames{};
        uint32_t m_currentFrameInFlight{ 0 };
    };
}
