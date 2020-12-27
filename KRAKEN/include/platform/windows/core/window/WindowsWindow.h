#pragma once
#include "KRAKEN/core/window/Window.h"
#include <Windows.h>

namespace KRKN::WINDOWS
{
    class KRAKEN_API WindowsWindow : public KRKN::Window
    {
    public:
        WindowsWindow(const KRKN::WindowData& windowData);
        virtual ~WindowsWindow() override;


        virtual void onUpdate() override;
        virtual void onResize(uint32_t width, uint32_t height) override;
        virtual void close() override;

        virtual void setWidth(uint32_t width) override;
        virtual uint32_t getWidth() const override;

        virtual void setHeight(uint32_t height) override;
        virtual uint32_t getHeight() const override;

        virtual void setTitle(const std::string& title) override;
        virtual const std::string& getTitle() const override;

        virtual void setVsync(const bool_t vsync) override;
        virtual bool_t isVsync() const override;

    private:
        LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

    private:
        HWND hwnd;
        HINSTANCE hInstance;
    };
}
