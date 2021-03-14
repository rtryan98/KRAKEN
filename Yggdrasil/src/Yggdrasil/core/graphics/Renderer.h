#pragma once
#include <vulkan/vulkan.h>
#include "Yggdrasil/core/graphics/Globals.h"
#include "Yggdrasil/Defines.h"
#include "Yggdrasil/core/util/Log.h"
#include "Yggdrasil/core/graphics/Util.h"
#include "Yggdrasil/core/graphics/Context.h"
#include "Yggdrasil/Types.h"
#include "Yggdrasil/core/memory/Pool.h"
#include "Yggdrasil/core/graphics/memory/Buffer.h"
#include "Yggdrasil/core/graphics/memory/Image.h"

#include <queue>

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
        void uploadStagedData();

        const graphics::Context& getContext() const;
        const PerFrame& getPerFrameData() const;

    private:
        void createPipeline();
        void acquirePerFrameData();
        void createDescriptorSetLayout();
        void createDescriptorPool();
        void freeDescriptorPool();

    private:
        void stageBufferCopy(memory::Buffer* src, memory::Buffer* dst);

    private:
        graphics::Context context{};
        VkPipelineLayout pipelineLayout{};
        VkPipeline pipeline{};
        PerFrame perFrame{};
        VkDescriptorSetLayout descriptorSetLayout{};
        VkDescriptorPool descriptorPool{};
        std::vector<VkDescriptorSet> descriptorSets{};

        memory::Buffer* uniformBuffer{};
        memory::Buffer* vertexBuffer{};
        memory::Buffer* stagingBufferTest{};

    private:
        yggdrasil::memory::Pool<memory::Buffer, 8192> buffers{};
        yggdrasil::memory::Pool<memory::Image, 8192> images{};
        std::queue<memory::BufferCopy> bufferCopyQueue{};
    };
}
