#include <Yggdrasil/Yggdrasil.h>
#include <Yggdrasil/Common/Window/Window.h>
#include <cstdint>

int32_t main()
{
    Ygg::InitInfo initInfo{};
    initInfo.windowCreateInfo.title = "Yggdrasil Sandbox";

    Ygg::Init(&initInfo);
    Ygg::Run();
    Ygg::Shutdown();
    return 0;
}
