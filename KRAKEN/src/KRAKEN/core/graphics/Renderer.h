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

        VkInstance getInstance() const;

    private:
        void setupDebugMessenger();
        void createCommandPool();

    private:
        vulkan::Context context{};

        VkSemaphore acquireSemaphore{};
        VkSemaphore releaseSemaphore{};
        VkCommandPool commandPool{};
        VkCommandBuffer commandBuffer{};
        VkFence submitFence{};
    };
}
