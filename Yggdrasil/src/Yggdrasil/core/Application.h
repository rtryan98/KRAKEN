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
        bool_t imguiEnabled{ true };
        bool_t decorated{ true };
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

        uint32_t getFramesPerSecond() const;
        float_t getCpuFrametime() const;

        Window* const getWindow() const;
        LayerStack& getLayerStack();
    private:
        Window* window;
        LayerStack layerStack{};
        const bool_t imguiEnabled;
        uint32_t framesPerSecond{ 0 };
        float_t cpuFrameTime{ 0.0f };
    };
}
