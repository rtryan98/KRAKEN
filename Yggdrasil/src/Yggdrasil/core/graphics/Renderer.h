#pragma once
#include <vulkan/vulkan.h>
#include "Yggdrasil/core/graphics/vulkan/Globals.h"
#include "Yggdrasil/Defines.h"
#include <functional>
#include "Yggdrasil/core/util/Log.h"
#include "Yggdrasil/core/graphics/vulkan/Util.h"
#include "Yggdrasil/core/graphics/vulkan/Context.h"

namespace yggdrasil
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
        void createShaderModules();
        void createPipeline();

    private:
        vulkan::Context context{};
        VkCommandBuffer commandBuffer{};
        VkRenderPass renderPass{};
        std::vector<VkFramebuffer> framebuffers{};
        VkSemaphore acquireSemaphore{};
        VkSemaphore releaseSemaphore{};
        VkShaderModule frag{};
        VkShaderModule vert{};
        VkPipelineLayout pipelineLayout{};
        VkPipeline pipeline{};
    };
}
