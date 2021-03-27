#pragma once
#include "Yggdrasil/Defines.h"
#if YGGDRASIL_PLATFORM_WINDOWS
#include "Yggdrasil/core/window/Window.h"
#include <Windows.h>

namespace ygg::windows
{
    class WindowsWindow : public ygg::Window
    {
    public:
        WindowsWindow(const ygg::WindowData& windowData);
        virtual ~WindowsWindow() override;

        virtual void getRequiredPlatformExtensions(uint32_t* count, const char*** platformExtensions) const override;

        virtual VkSurfaceKHR getSurface(VkInstance instance) const override;

    private:
        HWND hwnd;
        HINSTANCE hInstance;
    };
}
#endif
