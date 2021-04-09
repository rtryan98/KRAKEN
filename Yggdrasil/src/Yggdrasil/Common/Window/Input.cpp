#include "Yggdrasil/pch.h"
#include "Yggdrasil/Common/Window/Input.h"

#include <Windows.h>

namespace Ygg::Input
{
    bool IsKeyPressed(EKeyCode keyCode) noexcept
    {
        return (::GetKeyState(keyCode) & 0x8000);
    }

    bool IsKeyReleased(EKeyCode keyCode) noexcept
    {
        return (::GetKeyState(keyCode) & 0x0000);
    }

    bool IsKeyToggled(EKeyCode keyCode) noexcept
    {
        return (::GetKeyState(keyCode) & 0x0001);
    }

    bool IsMouseButtonPressed(EMouseButton button) noexcept
    {
        return (::GetKeyState(button) & 0x8000);
    }

    bool IsMouseButtonReleased(EMouseButton button) noexcept
    {
        return (::GetKeyState(button) & 0x0000);
    }

    bool IsMouseButtonToggled(EMouseButton button) noexcept
    {
        return (::GetKeyState(button) & 0x0001);
    }
}
