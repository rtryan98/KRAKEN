#pragma once
#include "Yggdrasil/RenderEngine/GraphicsContext.h"

#include <vector>
#include <deque>
#include <functional>

namespace Ygg
{
    class CRenderEngine
    {
    public:
        void Init();
        void Render();
        void Shutdown();
        const CGraphicsContext& GetGraphicsContext() const;
        void PushObjectDeletion(std::function<void()>&& mFunction);

    private:
        void InitPerFrameStruct();

    private:
        CGraphicsContext m_context{};

        struct SPerFrame
        {
            VkCommandPool cmdPool;
            VkCommandBuffer cmdBuffer;
            VkFence imageAcquireFence;
            VkSemaphore acquireSemaphore;
            VkSemaphore releaseSemaphore;
        };
        std::vector<SPerFrame> m_frames{};
        uint32_t m_currentFrameInFlight{ 0 };
        uint32_t m_maxFramesInFlight{ 0 };
        std::deque<std::function<void()>> m_deletionQueue;
    };
}
