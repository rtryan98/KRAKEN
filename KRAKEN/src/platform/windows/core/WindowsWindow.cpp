#include "platform/windows/core/window/WindowsWindow.h"
#if KRAKEN_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "KRAKEN/core/event/WindowEvent.h"
#include "KRAKEN/core/event/MouseEvent.h"
#include "KRAKEN/core/event/KeyboardEvent.h"
#include "KRAKEN/core/Globals.h"
#include "KRAKEN/core/graphics/Renderer.h"
#include <vulkan/vulkan_win32.h>

namespace kraken::windows
{
    WindowsWindow::WindowsWindow(const kraken::WindowData& windowData)
        : Window{ windowData }, hInstance{ GetModuleHandle( NULL ) }
    {
        KRAKEN_ASSERT(glfwInit());
        KRAKEN_ASSERT_VALUE(this->windowData.function != nullptr);

        glfwWindow = glfwCreateWindow(this->windowData.width, this->windowData.height, this->windowData.title.c_str(), nullptr, nullptr);
        KRAKEN_ASSERT_VALUE(glfwWindow);
        glfwSetWindowUserPointer(glfwWindow, &this->windowData);

        glfwSetWindowCloseCallback(glfwWindow,
            [](GLFWwindow* pWindow){
                WindowData* data{ static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)) };
                WindowCloseEvent evt{};
                data->function(evt);
            });
        glfwSetKeyCallback(glfwWindow, 
            [](GLFWwindow* pWindow, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
                KRAKEN_UNUSED_VARIABLE(scancode);
                KRAKEN_UNUSED_VARIABLE(mods);
                WindowData* data{ static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)) };
                switch (action)
                {
                case GLFW_PRESS:
                {
                    KeyPressEvent evt{ static_cast<uint32_t>(key) };
                    data->function(evt);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleaseEvent evt{ static_cast<uint32_t>(key) };
                    data->function(evt);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyRepeatEvent evt{ static_cast<uint32_t>(key), 1 }; // HACK: hardcoded.
                    data->function(evt);
                    break;
                }
                }
            });
        glfwSetMouseButtonCallback(glfwWindow, 
            [](GLFWwindow* pWindow, int32_t button, int32_t action, int32_t mods) {
                KRAKEN_UNUSED_VARIABLE(mods);
                WindowData* data{ static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)) };
                switch (action)
                {
                case GLFW_PRESS:
                {
                    MouseButtonPressEvent evt{ static_cast<uint32_t>(button) };
                    data->function(evt);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleaseEvent evt{ static_cast<uint32_t>(button) };
                    data->function(evt);
                    break;
                }
                }
            });
        glfwSetWindowSizeCallback(glfwWindow,
            [](GLFWwindow* pWindow, int32_t width, int32_t height) {
                KRAKEN_ASSERT_VALUE(width > 0);
                KRAKEN_ASSERT_VALUE(height > 0);
                WindowData* data{ static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)) };
                data->width = width;
                data->height = height;
                WindowResizeEvent evt{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
                data->function(evt);
            });
        glfwSetCursorPosCallback(glfwWindow,
            [](GLFWwindow* pWindow, double_t xPos, double_t yPos) {
                WindowData* data{ static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)) };
                MouseMoveEvent evt{ xPos, yPos };
                data->function(evt);
            });
        glfwSetScrollCallback(glfwWindow,
            [](GLFWwindow* pWindow, double_t xOffset, double_t yOffset) {
                WindowData* data{ static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)) };
                MouseScrollEvent evt{ xOffset, yOffset };
                data->function(evt);
            });

        glfwShowWindow(glfwWindow);
        glfwFocusWindow(glfwWindow);
        hwnd = glfwGetWin32Window(glfwWindow);
        isRun = true;
    }

    WindowsWindow::~WindowsWindow()
    {
        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
    }


    void WindowsWindow::onUpdate()
    {
        globals::CURRENT_FRAME_TIME = static_cast<float>(glfwGetTime());
        globals::DELTA_FRAME_TIME = globals::CURRENT_FRAME_TIME - globals::LAST_FRAME_TIME;
        globals::LAST_FRAME_TIME = globals::CURRENT_FRAME_TIME;
        glfwPollEvents();
    }

    void WindowsWindow::onResize(uint32_t width, uint32_t height)
    {
        this->windowData.width = width;
        this->windowData.height = height;
    }

    void WindowsWindow::close()
    {
        this->isRun = false;
    }

    /// Getters and Setters

    void WindowsWindow::setWidth(uint32_t width)
    {
        this->windowData.width = width;
    }

    uint32_t WindowsWindow::getWidth() const
    {
        return this->windowData.width;
    }


    void WindowsWindow::setHeight(uint32_t height)
    {
        this->windowData.height = height;
    }

    uint32_t WindowsWindow::getHeight() const
    {
        return this->windowData.height;
    }


    void WindowsWindow::setTitle(const std::string& title)
    {
        this->windowData.title = title;
    }

    const std::string& WindowsWindow::getTitle() const
    {
        return this->windowData.title;
    }


    void WindowsWindow::setVsync(const bool_t vsync)
    {
        this->windowData.vsync = vsync;
    }

    bool_t WindowsWindow::isVsync() const
    {
        return this->windowData.vsync;
    }

    GLFWwindow* WindowsWindow::getNativeWindow() const
    {
        return glfwWindow;
    }

    void WindowsWindow::getRequiredPlatformExtensions(uint32_t* count, const char*** platformExtensions) const
    {
        *platformExtensions = glfwGetRequiredInstanceExtensions(count);
    }

    VkSurfaceKHR WindowsWindow::getSurface() const
    {
        VkSurfaceKHR result{ 0 };
        VkWin32SurfaceCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
        createInfo.hwnd = hwnd;
        createInfo.hinstance = hInstance;
        vkCreateWin32SurfaceKHR(vulkan::INSTANCE, &createInfo, vulkan::VK_CPU_ALLOCATOR, &result);
        KRAKEN_ASSERT_VALUE(result != VK_NULL_HANDLE);
        return result;
    }
}
#endif
