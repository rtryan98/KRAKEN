#include "Yggdrasil/pch.h"
#include "Yggdrasil/Common/Window/Input.h"

#include <Windows.h>

namespace Ygg
{
    bool IsKeyPressed(KeyCode keyCode) noexcept
    {
        return (::GetKeyState(keyCode) & 0x8000);
    }

    bool IsKeyReleased(KeyCode keyCode) noexcept
    {
        return (::GetKeyState(keyCode) & 0x0000);
    }

    bool IsKeyToggled(KeyCode keyCode) noexcept
    {
        return (::GetKeyState(keyCode) & 0x0001);
    }

    bool IsMouseButtonPressed(MouseButton button) noexcept
    {
        return (::GetKeyState(button) & 0x8000);
    }

    bool IsMouseButtonReleased(MouseButton button) noexcept
    {
        return (::GetKeyState(button) & 0x0000);
    }

    bool IsMouseButtonToggled(MouseButton button) noexcept
    {
        return (::GetKeyState(button) & 0x0001);
    }
}
