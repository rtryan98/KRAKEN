#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/Globals.h"

namespace ygg::globals
{
    /// Engine Globals
    ygg::Application* APPLICATION{ nullptr };
    ygg::graphics::GraphicsEngine* GRAPHICS_ENGINE{ nullptr };

    float_t DELTA_FRAME_TIME{ 0.0f };
    double_t LAST_FRAME_TIME{ 0.0f };
    double_t CURRENT_FRAME_TIME{ 0.0f };
}
