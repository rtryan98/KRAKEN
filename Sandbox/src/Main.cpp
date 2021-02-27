#include <Yggdrasil/Yggdrasil.h>
#include "layers/SandboxLayer.h"

int32_t main()
{
    yggdrasil::ApplicationCreateInfo createInfo{};
    createInfo.title = "Yggdrasil Sandbox";
    createInfo.imguiEnabled = true;
    createInfo.decorated = true;
    yggdrasil::Application app{ createInfo };
    app.getLayerStack().pushOverlay( new SandboxLayer("Sandbox Layer") );
    app.run();

    return 0;
}
