#include <KRAKEN/KRAKEN.h>

int32_t main()
{
    kraken::Logger::init();
    kraken::WindowData windowData{};
    kraken::Window* window{ kraken::Window::createWindow(windowData) };
    while (window->isRunning())
    {
        window->onUpdate();
    }
    kraken::Logger::free();
    return 0;
}
