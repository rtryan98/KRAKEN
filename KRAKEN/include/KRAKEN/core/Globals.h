#pragma once
#include "KRAKEN/Types.h"

namespace kraken
{
    class Application;
    class Renderer;
}

namespace kraken::constants
{
    constexpr uint32_t VERSION_MAJOR{ 1 };
    constexpr uint32_t VERSION_MINOR{ 0 };
    constexpr uint32_t VERSION_PATCH{ 0 };
    
    constexpr char* ENGINE_NAME{ "KRAKEN Engine" };
}

namespace kraken::globals
{
    extern kraken::Application* APPLICATION;
    extern Renderer* RENDERER;

    extern float_t DELTA_FRAME_TIME;
    extern float_t LAST_FRAME_TIME;
    extern float_t CURRENT_FRAME_TIME;
}
