#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/Application.h"
#include "Yggdrasil/core/Globals.h"
#include "Yggdrasil/core/event/WindowEvent.h"
#include "Yggdrasil/core/input/Input.h"
#include "Yggdrasil/core/graphics/Renderer.h"

namespace yggdrasil
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
        globals::RENDERER->init(*this->window);
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
                YGGDRASIL_UNUSED_VARIABLE(evt);
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
            window->onUpdate();
            onUpdate();
            globals::RENDERER->onUpdate();
        }
    }

    Window* const Application::getWindow() const
    {
        return this->window;
    }
}
