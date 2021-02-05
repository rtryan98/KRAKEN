#include "KRAKEN/pch.h"

#include "KRAKEN/core/input/Input.h"
#include "KRAKEN/core/Globals.h"
#include "KRAKEN/core/Application.h"
#include <GLFW/glfw3.h>

namespace kraken
{
    namespace input
    {
        bool_t isKeyPressed(uint32_t keycode)
        {
            return glfwGetKey(kraken::globals::APPLICATION->getWindow()->getNativeWindow(), keycode);
        }

        bool_t isMouseButtonPressed(uint32_t mouseButton)
        {
            return glfwGetMouseButton(kraken::globals::APPLICATION->getWindow()->getNativeWindow(), mouseButton);
        }

        glm::vec2 getMousePosition()
        {
            glm::dvec2 result{};
            glfwGetCursorPos(kraken::globals::APPLICATION->getWindow()->getNativeWindow(), &result.x, &result.y);
            return glm::vec2{ static_cast<float>(result.x), static_cast<float>(result.y) };
        }
    }
}
