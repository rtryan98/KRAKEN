#include <Yggdrasil/Yggdrasil.h>
#include "layers/SandboxLayer.h"

int32_t main()
{
    ygg::ApplicationCreateInfo createInfo{};
    createInfo.width = 1920;
    createInfo.height = 1080;
    createInfo.title = "Yggdrasil Sandbox";
    createInfo.imguiEnabled = true;
    createInfo.decorated = true;
    ygg::Application app{ createInfo };
    app.getLayerStack().pushOverlay( new SandboxLayer("Sandbox Layer") );
    app.run();

    return 0;
}
