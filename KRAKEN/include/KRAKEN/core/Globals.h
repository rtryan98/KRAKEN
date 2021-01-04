#pragma once

struct VkAllocationCallbacks;

namespace kraken
{
    class Application;

    namespace globals
    {
        extern kraken::Application* APPLICATION;

        extern VkAllocationCallbacks* VK_CPU_ALLOCATOR;
    }
}
