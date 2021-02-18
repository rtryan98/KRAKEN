#include <Yggdrasil/Yggdrasil.h>

int32_t main()
{
    yggdrasil::ApplicationCreateInfo createInfo{};
    createInfo.title = "Yggdrasil Sandbox";
    yggdrasil::Application app{ createInfo };
    app.run();

    return 0;
}
