#include <KRAKEN/KRAKEN.h>

int32_t main()
{
    KRKN::WindowData windowData{};
    KRKN::Window* window{ KRKN::Window::createWindow(windowData) };
    while (window->isRunning())
    {
        window->onUpdate();
    }
    return 0;
}
