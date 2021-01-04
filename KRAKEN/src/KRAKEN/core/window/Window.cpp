#include "KRAKEN/core/window/Window.h"
#if KRAKEN_PLATFORM_WINDOWS
#include "platform/windows/core/window/WindowsWindow.h"
#endif

namespace kraken
{
    Window* Window::createWindow(const WindowData& windowData)
    {
#if KRAKEN_PLATFORM_WINDOWS
        return new kraken::windows::WindowsWindow(windowData);
#else
    #error "Non-Windows Window not yet impletmented!"
        return nullptr;
#endif
    }

    Window::Window(const WindowData& windowData)
        : windowData{ windowData }
    {}

    bool_t Window::isRunning() const
    {
        return this->isRun;
    }
}
