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
#include "Yggdrasil/core/graphics/resources/ResourceManager.h"

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

        const graphics::Context& getContext() const;
        const PerFrame& getPerFrameData() const;
        ResourceManager& getResourceManager();

        static GraphicsEngine* get();

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
        VkDescriptorSetLayout descriptorSetLayout{};
        VkDescriptorPool descriptorPool{};
        std::vector<VkDescriptorSet> descriptorSets{};

        memory::Buffer* uniformBuffer{};
        memory::Buffer* vertexBuffer{};
        memory::Texture* texture{};

    private:
        ResourceManager resourceManager{};
        static GraphicsEngine* instance;

    private:
        friend class BufferManager;
        friend class ResourceManager;
        friend class TextureManager;
    };
}