#include "Yggdrasil/pch.h"

#include "platform/windows/core/window/WindowsWindow.h"
#if YGGDRASIL_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "Yggdrasil/core/event/WindowEvent.h"
#include "Yggdrasil/core/event/MouseEvent.h"
#include "Yggdrasil/core/event/KeyboardEvent.h"
#include "Yggdrasil/core/Globals.h"
#include "Yggdrasil/core/graphics/Renderer.h"
#include <vulkan/vulkan_win32.h>

namespace yggdrasil::windows
{
    WindowsWindow::WindowsWindow(const yggdrasil::WindowData& windowData)
        : Window{ windowData }, hInstance{ GetModuleHandle( NULL ) }
    {
        YGGDRASIL_ASSERT(glfwInit());
        YGGDRASIL_ASSERT_VALUE(this->windowData.function != nullptr);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfwWindow = glfwCreateWindow(this->windowData.width, this->windowData.height, this->windowData.title.c_str(), nullptr, nullptr);
        YGGDRASIL_ASSERT_VALUE(glfwWindow);
        glfwSetWindowUserPointer(glfwWindow, &this->windowData);

        glfwSetWindowCloseCallback(glfwWindow,
            [](GLFWwindow* pWindow){
                WindowData* data{ static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)) };
                WindowCloseEvent evt{};
                data->function(evt);
            });
        glfwSetKeyCallback(glfwWindow, 
            [](GLFWwindow* pWindow, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
                YGGDRASIL_UNUSED_VARIABLE(scancode);
                YGGDRASIL_UNUSED_VARIABLE(mods);
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
                YGGDRASIL_UNUSED_VARIABLE(mods);
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
                YGGDRASIL_ASSERT_VALUE(width > 0);
                YGGDRASIL_ASSERT_VALUE(height > 0);
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

    void WindowsWindow::getRequiredPlatformExtensions(uint32_t* count, const char*** platformExtensions) const
    {
        *platformExtensions = glfwGetRequiredInstanceExtensions(count);
    }

    VkSurfaceKHR WindowsWindow::getSurface(VkInstance instance) const
    {
        VkSurfaceKHR result{ 0 };
        VkWin32SurfaceCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
        createInfo.hwnd = hwnd;
        createInfo.hinstance = hInstance;
        vkCreateWin32SurfaceKHR(instance, &createInfo, vulkan::VK_CPU_ALLOCATOR, &result);
        YGGDRASIL_ASSERT_VALUE(result != VK_NULL_HANDLE);
        return result;
    }
}
#endif
