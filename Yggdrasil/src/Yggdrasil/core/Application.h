#pragma once
#include "Yggdrasil/core/window/Window.h"
#include "Yggdrasil/core/util/LayerStack.h"

namespace yggdrasil
{
    struct ApplicationCreateInfo
    {
        uint32_t width{ 1920 };
        uint32_t height{ 1080 };
        std::string title{ "Yggdrasil Sandbox" };
    };

    class Application
    {
    public:
        Application(const ApplicationCreateInfo& createInfo);
        ~Application();

        void onEvent(Event& evt);
        void onUpdate();
        void onImguiUpdate();
        void run();

        Window* const getWindow() const;
        LayerStack& getLayerStack();
    private:
        Window* window;
        LayerStack layerStack{};
    };
}
