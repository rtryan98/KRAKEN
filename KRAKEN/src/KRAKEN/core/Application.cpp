#include "KRAKEN/core/Application.h"
#include "KRAKEN/core/Globals.h"
#include "KRAKEN/core/event/WindowEvent.h"
#include "KRAKEN/core/input/Input.h"
#include "KRAKEN/core/graphics/Renderer.h"

namespace kraken
{
    Application::Application(const ApplicationCreateInfo& createInfo)
        : window{ nullptr }
    {
        Logger::init();
        globals::APPLICATION = this;
        WindowData windowData{};
        if (createInfo.width > 0)
        {
            windowData.width = createInfo.width;
        }
        if (createInfo.height > 0)
        {
            windowData.height = createInfo.height;
        }
        windowData.title = createInfo.title;
        windowData.function = [this](auto&&... args) -> decltype(auto)
            {
            return this->onEvent(std::forward<decltype(args)>(args)...);
            };
        window = Window::createWindow( windowData );
        globals::RENDERER = new Renderer();
        globals::RENDERER->init();
    }

    Application::~Application()
    {
        globals::RENDERER->free();
        delete globals::RENDERER;
        delete window;
        Logger::free();
    }

    void Application::onEvent(Event& evt)
    {
        EventDispatcher dispatcher(evt);
        dispatcher.dispatch<WindowCloseEvent>(
            [&](WindowCloseEvent& evt) {
                KRAKEN_UNUSED_VARIABLE(evt);
                this->getWindow()->close();
                return false;
            });
    }

    void Application::onUpdate()
    {

    }

    void Application::run()
    {
        while (window->isRunning())
        {
            onUpdate();
            window->onUpdate();
        }
    }

    Window* const Application::getWindow() const
    {
        return this->window;
    }
}
