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

    struct PerFrame
    {
        VkCommandPool commandPool{};
        VkCommandBuffer commandBuffer{};
        VkImage swapchainImage{};
        VkImageView swapchainImageView{};
        VkFramebuffer framebuffer{};
    };

    class Renderer
    {
    public:
        Renderer() = default;
        ~Renderer() = default;

        void init(const Window& window);
        void free();
        void onUpdate();
        void present();
        void prepare();

        const vulkan::Context& getContext() const;
        VkRenderPass getRenderPass() const;
        const PerFrame& getPerFrameData() const;
        const std::vector<VkFramebuffer>& getFramebuffers() const;
        uint32_t getCurrentFrame() const;

    private:
        void createRenderPasses();
        void createFramebuffers();
        void createSyncObjects();
        void freeSyncObjects();
        void createShaderModules();
        void createPipeline();

        void acquirePerFrameData();

    private:
        vulkan::Context context{};
        VkRenderPass renderPass{};
        std::vector<VkFramebuffer> framebuffers{};
        VkSemaphore acquireSemaphore{};
        VkSemaphore releaseSemaphore{};
        VkShaderModule frag{};
        VkShaderModule vert{};
        VkPipelineLayout pipelineLayout{};
        VkPipeline pipeline{};
        uint32_t currentImage{ 0 };
        PerFrame perFrame{};
    };
}
