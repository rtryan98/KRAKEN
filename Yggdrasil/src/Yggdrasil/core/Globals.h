#pragma once
#include "Yggdrasil/Types.h"
#include "Yggdrasil/core/graphics/Renderer.h"

namespace yggdrasil
{
    class Application;
}

namespace yggdrasil::constants
{
    constexpr uint32_t VERSION_MAJOR{ 1 };
    constexpr uint32_t VERSION_MINOR{ 0 };
    constexpr uint32_t VERSION_PATCH{ 0 };
    
    constexpr char* ENGINE_NAME{ "Yggdrasil Engine" };
}

namespace yggdrasil::globals
{
    extern yggdrasil::Application* APPLICATION;
    extern yggdrasil::graphics::Renderer* RENDERER;

    extern float_t DELTA_FRAME_TIME;
    extern float_t LAST_FRAME_TIME;
    extern float_t CURRENT_FRAME_TIME;
}
