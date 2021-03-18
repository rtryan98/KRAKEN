#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/Application.h"
#include "Yggdrasil/core/Globals.h"
#include "Yggdrasil/core/window/Window.h"
#include "Yggdrasil/core/event/WindowEvent.h"
#include "Yggdrasil/core/input/Input.h"
#include "Yggdrasil/core/util/layers/DearImguiLayer.h"
#include "Yggdrasil/core/graphics/ShaderCompiler.h"
#include "Yggdrasil/core/graphics/GraphicsEngine.h"

#include <glfw/glfw3.h>

namespace yggdrasil
{
    Application::Application(const ApplicationCreateInfo& createInfo)
        : window{ nullptr }, imguiEnabled{ createInfo.imguiEnabled }
    {
        Logger::init();
        graphics::shadercompiler::init();
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
        windowData.decorated = createInfo.decorated;
        window = Window::createWindow( windowData );
        globals::GRAPHICS_ENGINE = new graphics::GraphicsEngine();
        globals::GRAPHICS_ENGINE->init(*this->window);
        ImguiLayer::init();
    }

    Application::~Application()
    {
        ImguiLayer::free();
        globals::GRAPHICS_ENGINE->free();
        delete globals::GRAPHICS_ENGINE;
        delete window;
        graphics::shadercompiler::free();
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
        for (Layer* layer : this->layerStack)
        {
            layer->onUpdate();
        }
    }

    void Application::onImguiUpdate()
    {
        for (Layer* layer : this->layerStack)
        {
            layer->onImguiUpdate();
        }
    }

    void Application::run()
    {
        float_t deltaAccumulator{ 1.0f };
        uint32_t frames{ 0 };
        while (window->isRunning())
        {
            globals::CURRENT_FRAME_TIME = glfwGetTime();
            globals::DELTA_FRAME_TIME = static_cast<float_t>(globals::CURRENT_FRAME_TIME - globals::LAST_FRAME_TIME);
            globals::LAST_FRAME_TIME = globals::CURRENT_FRAME_TIME;
            deltaAccumulator -= globals::DELTA_FRAME_TIME;
            if (deltaAccumulator < 0.0f)
            {
                this->framesPerSecond = frames;
                this->cpuFrameTime = 1.0f / static_cast<float>(this->framesPerSecond) * 1000;
                frames = 0;
                deltaAccumulator = 1.0f;
            }
            window->onUpdate();
            globals::GRAPHICS_ENGINE->prepare();
            onUpdate();
            globals::GRAPHICS_ENGINE->onUpdate(globals::DELTA_FRAME_TIME);
            if (this->imguiEnabled)
            {
                onImguiUpdate();
            }
            globals::GRAPHICS_ENGINE->present();
            frames++;
        }
        for (Layer* layer : this->layerStack)
        {
            YGGDRASIL_CORE_TRACE("Detach layer [app close] '{0}'", layer->getDebugName());
            layer->onDetachInternal();
        }
    }

    Window* const Application::getWindow() const
    {
        return this->window;
    }

    LayerStack& Application::getLayerStack()
    {
        return this->layerStack;
    }

    uint32_t Application::getFramesPerSecond() const
    {
        return this->framesPerSecond;
    }

    float_t Application::getCpuFrametime() const
    {
        return this->cpuFrameTime;
    }
}
