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
#include "Yggdrasil/core/graphics/memory/Texture.h"

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

    class GraphicsEngine
    {
    public:
        GraphicsEngine() = default;
        ~GraphicsEngine() = default;

        void init(const Window& window);
        void free();
        void onUpdate();
        void present();
        void prepare();
        void handleStagedData();
        void handleStagedBufferCopies();
        void handleStagedBufferToTextureCopies();

        const graphics::Context& getContext() const;
        const PerFrame& getPerFrameData() const;

    private:
        void createPipeline();
        void acquirePerFrameData();
        void createDescriptorSetLayout();
        void createDescriptorPool();
        void freeDescriptorPool();

    private:
        void stageBufferCopy(memory::Buffer* src, memory::Buffer* dst, uint64_t srcOffset = 0, uint64_t dstOffset = 0);
        void stageBufferToImageCopy(memory::Buffer* src, memory::Texture* dst,
            uint32_t srcOffset = 0, uint32_t dstOffsetX = 0, uint32_t dstOffsetY = 0, uint32_t dstOffsetZ = 0);

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
        memory::Buffer* stagingBuffer{};

    private:
        yggdrasil::memory::Pool<memory::Buffer, 8192> buffers{};
        yggdrasil::memory::Pool<memory::Texture, 8192> images{};
        std::queue<memory::BufferCopy> bufferCopyQueue{};
        std::queue<memory::BufferToTextureCopy> bufferToTextureCopyQueue{};
    };
}
