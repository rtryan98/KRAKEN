#pragma once
#include "KRAKEN/Defines.h"
#include "KRAKEN/Types.h"
#include <string>
#include "KRAKEN/core/event/Event.h"
#include <functional>
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace kraken
{
    struct WindowData
    {
        uint32_t width{ 1920u };
        uint32_t height{ 1080u };
        std::string title{ "KRAKEN Engine" };
        bool_t fullscreen{ false };
        bool_t vsync{ false };
        std::function<void(Event&)> function;
    };

    class Window
    {
    public:

        virtual ~Window() = default;

        void onUpdate();
        void onResize(uint32_t width, uint32_t height);
        void close();

        void setWidth(uint32_t width);
        uint32_t getWidth() const;

        void setHeight(uint32_t height);
        uint32_t getHeight() const;

        void setTitle(const std::string& title);
        const std::string& getTitle() const;

        void setVsync(const bool_t vsync);
        bool_t isVsync() const;

        GLFWwindow* getNativeWindow() const;

        virtual void getRequiredPlatformExtensions(uint32_t* count, const char*** platformExtensions) const = 0;
        virtual VkSurfaceKHR getSurface(VkInstance instance) const = 0;

        VkExtent2D getFramebufferSize() const;

        bool_t isRunning() const;

        /// <summary>
        /// Creates a platform-specific Window.
        /// </summary>
        /// <param name="windowData"> Data to create the Window with </param>
        /// <returns> Pointer to the platform specific Window </returns>
        static Window* createWindow(const WindowData& windowData);
    protected:
        Window(const WindowData& windowData);

    protected:
        WindowData windowData;
        bool_t isRun{ false };
        GLFWwindow* glfwWindow{ nullptr };
    };
}
