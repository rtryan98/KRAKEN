#pragma once
#include "KRAKEN/core/window/Window.h"

namespace kraken
{
    struct ApplicationCreateInfo
    {
        uint32_t width{ 1920 };
        uint32_t height{ 1080 };
        std::string title{ "KRAKEN Engine" };
    };

    class Application
    {
    public:
        Application(const ApplicationCreateInfo& createInfo);
        ~Application();

        void onEvent(Event& evt);
        void onUpdate();
        void run();

        Window* const getWindow() const;
    private:
        Window* window;
    };
}
