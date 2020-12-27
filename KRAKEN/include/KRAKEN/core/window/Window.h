#pragma once
#include "KRAKEN/Defines.h"
#include "KRAKEN/Types.h"
#include <string>

namespace KRKN
{
    struct WindowData
    {
        uint32_t width{ 1920u };
        uint32_t height{ 1080u };
        std::string title{ "KRAKEN Engine" };
        bool_t fullscreen{ false };
        bool_t vsync{ false };
    };

    class Window
    {
    public:

        virtual ~Window() = default;


        virtual void onUpdate() = 0;
        virtual void onResize(uint32_t width, uint32_t height) = 0;
        virtual void close() = 0;

        virtual void setWidth(uint32_t width) = 0;
        virtual uint32_t getWidth() const = 0;

        virtual void setHeight(uint32_t height) = 0;
        virtual uint32_t getHeight() const = 0;

        virtual void setTitle(const std::string& title) = 0;
        virtual const std::string& getTitle() const = 0;

        virtual void setVsync(const bool_t vsync) = 0;
        virtual bool_t isVsync() const = 0;

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
    };
}
