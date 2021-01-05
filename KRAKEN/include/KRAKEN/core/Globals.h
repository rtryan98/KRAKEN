#pragma once
#include "KRAKEN/Types.h"

struct VkAllocationCallbacks;

namespace kraken
{
    class Application;
    class Renderer;
}

namespace kraken::globals
{

    /// Engine Globals
    extern kraken::Application* APPLICATION;
    extern Renderer* RENDERER;

    extern float_t DELTA_FRAME_TIME;
    extern float_t LAST_FRAME_TIME;
    extern float_t CURRENT_FRAME_TIME;

    /// Vulkan Globals
    extern VkAllocationCallbacks* VK_CPU_ALLOCATOR;
}
