#include "Yggdrasil/pch.h"
#include "Yggdrasil/Common/Window/Window.h"
#ifdef YGG_PLATFORM_WINDOWS
    #include <Windows.h>
#endif
#include "Yggdrasil/Common/Engine.h"

namespace Ygg
{
    LRESULT CALLBACK WndProc(HWND m_hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CLOSE:
            CEngine::GetWindowNonConst().Destroy();
            ::DestroyWindow(m_hwnd);
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            break;
        default:
            return ::DefWindowProc(m_hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    void CWindow::Create(const SWindowCreateInfo& windowCreateInfo) noexcept
    {
        this->m_data.width = windowCreateInfo.width;
        this->m_data.height = windowCreateInfo.height;
        this->m_data.title = windowCreateInfo.title;

        ::WNDCLASSEX wc{};
        wc.cbSize = sizeof(::WNDCLASSEX);
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.style = 0;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = static_cast<::HINSTANCE>(this->m_hInstance);
        wc.hIcon = ::LoadIcon(nullptr, IDI_WINLOGO);
        wc.hIconSm = wc.hIcon;
        wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = this->m_data.title.c_str();

        ::RegisterClassEx(&wc);

        ::RECT wr{ 0, 0, static_cast<LONG>(this->m_data.width), static_cast<LONG>(this->m_data.height) };
        constexpr ::DWORD style{ WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX };

        this->m_hwnd = ::CreateWindowEx(0, wc.lpszClassName, wc.lpszClassName, style, 0, 0,
            wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr,
            ::GetModuleHandle(nullptr), 0);

        ::ShowWindow(static_cast<::HWND>(this->m_hwnd), SW_SHOWDEFAULT);
        ::SetForegroundWindow(static_cast<::HWND>(this->m_hwnd));
        ::SetFocus(static_cast<::HWND>(this->m_hwnd));
    }

    void CWindow::Destroy() noexcept
    {
        this->m_data.isClosed = true;
    }

    void CWindow::Update() noexcept
    {
        MSG msg;
        ZeroMemory(&msg, sizeof(MSG));
        if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    bool CWindow::IsClosed() noexcept
    {
        return this->m_data.isClosed;
    }

    void CWindow::SetTitle(const std::string& title) noexcept
    {
        this->m_data.title = title;
        ::SetWindowText(static_cast<::HWND>(this->m_hwnd), this->m_data.title.c_str());
    }
}
