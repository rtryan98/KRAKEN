#pragma once
#include <cstdint>
#include <cstdlib>
#include <string>

namespace Ygg
{
    struct WindowCreateInfo
    {
        uint32_t width{ 1920 };
        uint32_t height{ 1080 };
        std::string title{ "Yggdrasil Engine" };
    };

    class Window
    {
    public:
        void Create(const WindowCreateInfo* windowCreateInfo) noexcept;
        void Destroy() noexcept;
        void Update() noexcept;
        bool IsClosed() noexcept;

    private:
        using HINSTANCE = void*;
        HINSTANCE hInstance;
        using HWND = void*;
        HWND hwnd;

        struct WindowData
        {
            uint32_t width;
            uint32_t height;
            std::string title;
            bool isClosed;
        } data;
    };
}
