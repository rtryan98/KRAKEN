#pragma once
#include "Yggdrasil/core/input/InputCodes.h"
#include "Yggdrasil/Types.h"
#include <glm/glm.hpp>

namespace yggdrasil
{
    namespace input
    {
        bool_t isKeyPressed(uint32_t keycode);
        bool_t isMouseButtonPressed(uint32_t mouseButton);
        glm::vec2 getMousePosition();
    }
}
