#pragma once
#include <vulkan/vulkan.h>
#include "Yggdrasil/core/graphics/Globals.h"
#include "Yggdrasil/Defines.h"
#include <functional>
#include "Yggdrasil/core/util/Log.h"
#include "Yggdrasil/core/graphics/Util.h"
#include "Yggdrasil/core/graphics/Context.h"
#include "Yggdrasil/Types.h"
#include "Yggdrasil/core/graphics/memory/Resource.h"
#include "Yggdrasil/core/graphics/memory/UniformBuffer.h"

class yggdrasil::Window;

namespace yggdrasil::graphics
{
    struct PerFrame
    {
        VkCommandPool commandPool{};
        VkCommandBuffer commandBuffer{};
        VkImage swapchainImage{};
        VkImageView swapchainImageView{};
        VkFramebuffer framebuffer{};
        VkSemaphore acquireSemaphore{};
        VkSemaphore releaseSemaphore{};
        VkFence acquireFence{};
        uint32_t frame{ 0 };
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

        const graphics::Context& getContext() const;
        const PerFrame& getPerFrameData() const;

    private:
        void createPipeline();
        void acquirePerFrameData();
        void createDescriptorSetLayout();
        void createDescriptorPool();
        void freeDescriptorPool();

    private:
        graphics::Context context{};
        VkPipelineLayout pipelineLayout{};
        VkPipeline pipeline{};
        PerFrame perFrame{};
        memory::AllocatedBuffer vbo{};
        memory::UniformBuffer ubo{};
        VkDescriptorSetLayout descriptorSetLayout{};
        VkDescriptorPool descriptorPool{};
        std::vector<VkDescriptorSet> descriptorSets{};
    };
}
