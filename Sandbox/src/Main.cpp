#include <KRAKEN/KRAKEN.h>

int32_t main()
{
    kraken::WindowData windowData{};
    kraken::Window* window{ kraken::Window::createWindow(windowData) };
    while (window->isRunning())
    {
        window->onUpdate();
    }
    return 0;
}
