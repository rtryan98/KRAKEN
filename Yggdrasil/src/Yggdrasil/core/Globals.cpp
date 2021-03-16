#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/Globals.h"

namespace yggdrasil::globals
{
    /// Engine Globals
    yggdrasil::Application* APPLICATION{ nullptr };
    yggdrasil::graphics::GraphicsEngine* GRAPHICS_ENGINE{ nullptr };

    float_t DELTA_FRAME_TIME{ 0.0f };
    float_t LAST_FRAME_TIME{ 0.0f };
    float_t CURRENT_FRAME_TIME{ 0.0f };
}
