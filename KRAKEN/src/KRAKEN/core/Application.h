#pragma once
#include "KRAKEN/core/window/Window.h"

namespace kraken
{
    struct ApplicationCreateInfo
    {
        uint32_t width;
        uint32_t height;
        std::string title;
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
