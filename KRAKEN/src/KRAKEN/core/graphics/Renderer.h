#pragma once
#include <vulkan/vulkan.h>
#include "KRAKEN/core/graphics/vulkan/Globals.h"
#include "KRAKEN/Defines.h"
#include <functional>
#include "KRAKEN/core/util/Log.h"
#include "KRAKEN/core/graphics/vulkan/Util.h"
#include "KRAKEN/core/graphics/vulkan/Context.h"

namespace kraken
{
    class Window;

    class Renderer
    {
    public:
        Renderer() = default;
        ~Renderer() = default;

        void init(const Window& window);
        void free();
        void onUpdate();

        const vulkan::Context& getContext() const;

    private:
        void createRenderPasses();
        void createFramebuffers();
        void createSyncObjects();
        void freeSyncObjects();

    private:
        vulkan::Context context{};
        VkCommandBuffer commandBuffer{};
        VkRenderPass renderPass{};
        std::vector<VkFramebuffer> framebuffers{};
        VkSemaphore acquireSemaphore{};
        VkSemaphore releaseSemaphore{};
    };
}
