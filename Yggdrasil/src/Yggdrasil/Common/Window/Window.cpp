#include "Yggdrasil/pch.h"
#include "Yggdrasil/Common/Window/Window.h"
#ifdef YGG_PLATFORM_WINDOWS
    #include <Windows.h>
#endif
#include "Yggdrasil/Common/Engine.h"

namespace Ygg
{
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CLOSE:
            Engine::GetWindow().Destroy();
            ::DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            break;
        default:
            return ::DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    void Window::Create(const WindowCreateInfo& windowCreateInfo) noexcept
    {
        this->data.width = windowCreateInfo.width;
        this->data.height = windowCreateInfo.height;
        this->data.title = windowCreateInfo.title;

        ::WNDCLASSEX wc{};
        wc.cbSize = sizeof(::WNDCLASSEX);
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.style = 0;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = static_cast<::HINSTANCE>(this->hInstance);
        wc.hIcon = ::LoadIcon(nullptr, IDI_WINLOGO);
        wc.hIconSm = wc.hIcon;
        wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = this->data.title.c_str();

        ::RegisterClassEx(&wc);

        ::RECT wr{ 0, 0, static_cast<LONG>(this->data.width), static_cast<LONG>(this->data.height) };
        constexpr ::DWORD style{ WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX };

        this->hwnd = ::CreateWindowEx(0, wc.lpszClassName, wc.lpszClassName, style, 0, 0,
            wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr,
            ::GetModuleHandle(nullptr), 0);

        ::ShowWindow(static_cast<::HWND>(this->hwnd), SW_SHOWDEFAULT);
        ::SetForegroundWindow(static_cast<::HWND>(this->hwnd));
        ::SetFocus(static_cast<::HWND>(this->hwnd));
    }

    void Window::Destroy() noexcept
    {
        this->data.isClosed = true;
    }

    void Window::Update() noexcept
    {
        MSG msg;
        ZeroMemory(&msg, sizeof(MSG));
        if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    bool Window::IsClosed() noexcept
    {
        return this->data.isClosed;
    }

    void Window::SetTitle(const std::string& title) noexcept
    {
        this->data.title = title;
        ::SetWindowText(static_cast<::HWND>(this->hwnd), this->data.title.c_str());
    }
}
