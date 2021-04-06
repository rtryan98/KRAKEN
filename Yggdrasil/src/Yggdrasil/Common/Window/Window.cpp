#include "Yggdrasil/pch.h"
#include "Yggdrasil/Common/Window/Window.h"
#ifdef YGG_PLATFORM_WINDOWS
    #include <Windows.h>
#endif

namespace Ygg
{
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CLOSE:
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

    void Window::Create(const WindowCreateInfo* windowCreateInfo) noexcept
    {
        this->data.width = windowCreateInfo->width;
        this->data.height = windowCreateInfo->height;
        this->data.title = windowCreateInfo->title;
        this->data.wCharTitle = std::wstring(windowCreateInfo->title.begin(), windowCreateInfo->title.end());

        ::WNDCLASSEX wc{};
        wc.cbSize = sizeof(::WNDCLASSEX);
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.style = 0;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = static_cast<::HINSTANCE>(this->hInstance);
        wc.hIcon = ::LoadIconW(nullptr, IDI_WINLOGO);
        wc.hIconSm = wc.hIcon;
        wc.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = this->data.wCharTitle.c_str();

        ::RegisterClassExW(&wc);

        ::RECT wr{ 0, 0, static_cast<LONG>(this->data.width), static_cast<LONG>(this->data.height) };
        constexpr ::DWORD style{ WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX };

        this->hwnd = ::CreateWindowExW(0, wc.lpszClassName, wc.lpszClassName, style, 0, 0,
            wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr,
            ::GetModuleHandleW(nullptr), 0);

        ::ShowWindow(static_cast<::HWND>(this->hwnd), SW_SHOWDEFAULT);
        ::SetForegroundWindow(static_cast<::HWND>(this->hwnd));
        ::SetFocus(static_cast<::HWND>(this->hwnd));
    }

    void Window::Destroy() noexcept
    {
        ::DestroyWindow(static_cast<::HWND>(this->hwnd));
    }

    void Window::Update() noexcept
    {
        MSG msg;
        ZeroMemory(&msg, sizeof(MSG));
        if (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
    }
}
