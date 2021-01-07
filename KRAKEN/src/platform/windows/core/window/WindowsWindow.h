#pragma once
#include "KRAKEN/Defines.h"
#if KRAKEN_PLATFORM_WINDOWS
#include "KRAKEN/core/window/Window.h"
#include <Windows.h>

namespace kraken::windows
{
    class WindowsWindow : public kraken::Window
    {
    public:
        WindowsWindow(const kraken::WindowData& windowData);
        virtual ~WindowsWindow() override;

        virtual void getRequiredPlatformExtensions(uint32_t* count, const char*** platformExtensions) const override;

        virtual VkSurfaceKHR getSurface(VkInstance instance) const override;

    private:
        HWND hwnd;
        HINSTANCE hInstance;
    };
}
#endif
