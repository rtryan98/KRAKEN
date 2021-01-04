#include "platform/windows/core/window/WindowsWindow.h"

namespace kraken::windows
{
    Window* window{ nullptr };

    /// Event dispatching
    LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CREATE:
        {
            return 0;
        }
        case WM_QUIT:
        {
            return 0;
        }
        case WM_CLOSE:
        {
            window->close();
            return 0;
        }
        case WM_DESTROY:
        {
            return 0;
        }
        default:
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        }
    }

    WindowsWindow::WindowsWindow(const kraken::WindowData& windowData)
        : Window{ windowData }, hInstance{ GetModuleHandle( NULL ) }
    {
        WNDCLASSEX wc{};
        wc.cbClsExtra = NULL;
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.cbWndExtra = NULL;
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        wc.hInstance = NULL;
        wc.lpszClassName = this->windowData.title.c_str();
        wc.lpszMenuName = NULL;
        wc.lpfnWndProc = kraken::windows::WndProc;
        wc.style = NULL;

        RegisterClassEx(&wc);

        hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, this->windowData.title.c_str(), this->windowData.title.c_str(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, this->windowData.width, this->windowData.height,
            NULL, NULL, NULL, NULL);

        KRAKEN_ASSERT_VALUE(hwnd);

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        SetFocus(hwnd);
        SetForegroundWindow(hwnd);

        isRun = true;
        window = this; // HACK: potential memory leak if multiple windows are active. Should never happen.
    }

    WindowsWindow::~WindowsWindow()
    {
        DestroyWindow(hwnd);
    }


    void WindowsWindow::onUpdate()
    {
        MSG message;
        ZeroMemory(&message, sizeof(MSG));

        if (PeekMessage(&message, NULL, 0u, 0u, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }

    void WindowsWindow::onResize(uint32_t width, uint32_t height)
    {
        this->windowData.width = width;
        this->windowData.height = height;
    }

    void WindowsWindow::close()
    {
        this->isRun = false;
        CloseWindow(hwnd);
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

}
