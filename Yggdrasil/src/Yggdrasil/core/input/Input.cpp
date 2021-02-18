#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/input/Input.h"
#include "Yggdrasil/core/Globals.h"
#include "Yggdrasil/core/Application.h"
#include <GLFW/glfw3.h>

namespace yggdrasil
{
    namespace input
    {
        bool_t isKeyPressed(uint32_t keycode)
        {
            return glfwGetKey(yggdrasil::globals::APPLICATION->getWindow()->getNativeWindow(), keycode);
        }

        bool_t isMouseButtonPressed(uint32_t mouseButton)
        {
            return glfwGetMouseButton(yggdrasil::globals::APPLICATION->getWindow()->getNativeWindow(), mouseButton);
        }

        glm::vec2 getMousePosition()
        {
            glm::dvec2 result{};
            glfwGetCursorPos(yggdrasil::globals::APPLICATION->getWindow()->getNativeWindow(), &result.x, &result.y);
            return glm::vec2{ static_cast<float>(result.x), static_cast<float>(result.y) };
        }
    }
}
