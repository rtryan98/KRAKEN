#include "KRAKEN/pch.h"

#include "KRAKEN/core/window/Window.h"
#include "KRAKEN/core/Globals.h"
#if KRAKEN_PLATFORM_WINDOWS
#include "platform/windows/core/window/WindowsWindow.h"
#endif
#include <GLFW/glfw3.h>

namespace kraken
{
    Window* Window::createWindow(const WindowData& windowData)
    {
#if KRAKEN_PLATFORM_WINDOWS
        return new kraken::windows::WindowsWindow(windowData);
#else
    #error "Non-Windows Window not yet impletmented!"
        return nullptr;
#endif
    }

    Window::Window(const WindowData& windowData)
        : windowData{ windowData }
    {}

    bool_t Window::isRunning() const
    {
        return this->isRun;
    }

    void Window::onResize(uint32_t width, uint32_t height)
    {
        this->windowData.width = width;
        this->windowData.height = height;
    }

    void Window::onUpdate()
    {
        globals::CURRENT_FRAME_TIME = static_cast<float>(glfwGetTime());
        globals::DELTA_FRAME_TIME = globals::CURRENT_FRAME_TIME - globals::LAST_FRAME_TIME;
        globals::LAST_FRAME_TIME = globals::CURRENT_FRAME_TIME;
        glfwPollEvents();
    }

    void Window::close()
    {
        this->isRun = false;
    }

    /// Getters and Setters

    void Window::setWidth(uint32_t width)
    {
        this->windowData.width = width;
    }

    uint32_t Window::getWidth() const
    {
        return this->windowData.width;
    }


    void Window::setHeight(uint32_t height)
    {
        this->windowData.height = height;
    }

    uint32_t Window::getHeight() const
    {
        return this->windowData.height;
    }


    void Window::setTitle(const std::string& title)
    {
        this->windowData.title = title;
    }

    const std::string& Window::getTitle() const
    {
        return this->windowData.title;
    }


    void Window::setVsync(const bool_t vsync)
    {
        this->windowData.vsync = vsync;
    }

    bool_t Window::isVsync() const
    {
        return this->windowData.vsync;
    }

    GLFWwindow* Window::getNativeWindow() const
    {
        return this->glfwWindow;
    }

    VkExtent2D Window::getFramebufferSize() const
    {
        int32_t width{}, height{};
        glfwGetFramebufferSize(this->glfwWindow, &width, &height);
        return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    }
}
