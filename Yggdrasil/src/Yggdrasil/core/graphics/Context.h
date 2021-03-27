#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "Yggdrasil/core/graphics/Device.h"
#include "Yggdrasil/core/graphics/Screen.h"

namespace ygg::graphics
{
    struct Context
    {
        VkInstance instance;

        graphics::Device device{};
        graphics::Screen screen{};

        std::vector<VkCommandPool> commandPools{};
        std::vector<VkCommandBuffer> commandBuffers{};

        struct Sync
        {
            std::vector<VkSemaphore> acquireSemaphores{};
            std::vector<VkSemaphore> releaseSemaphores{};
            std::vector<VkFence> acquireFences{};
        } syncObjects;
    };

    void initContext(Context& context);
    void freeContext(Context& context);
}
