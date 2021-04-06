#include <Yggdrasil/Yggdrasil.h>
#include <Yggdrasil/Common/Window/Window.h>
#include <cstdint>

int32_t main()
{
    Ygg::Window win{};
    Ygg::WindowCreateInfo winCreateInfo{};
    win.Create(&winCreateInfo);
    while (true)
    {
        win.Update();
    }
    return 0;
}
