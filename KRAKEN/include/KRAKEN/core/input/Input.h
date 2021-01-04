#pragma once
#include "KRAKEN/core/input/InputCodes.h"
#include "KRAKEN/Types.h"
#include <glm/glm.hpp>

namespace kraken
{
    namespace input
    {
        bool_t isKeyPressed(uint32_t keycode);
        bool_t isMouseButtonPressed(uint32_t mouseButton);
        glm::vec2 getMousePosition();
    }
}
