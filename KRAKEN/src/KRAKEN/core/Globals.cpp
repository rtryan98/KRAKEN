#include "KRAKEN/core/Globals.h"

namespace kraken::globals
{
    /// Engine Globals
    kraken::Application* APPLICATION{ nullptr };
    kraken::Renderer* RENDERER{ nullptr };

    float_t DELTA_FRAME_TIME{ 0.0f };
    float_t LAST_FRAME_TIME{ 0.0f };
    float_t CURRENT_FRAME_TIME{ 0.0f };
}
