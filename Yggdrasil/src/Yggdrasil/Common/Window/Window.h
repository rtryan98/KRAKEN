// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#include <cstdint>
#include <cstdlib>
#include <string>

namespace Ygg
{
    struct SWindowCreateInfo
    {
        uint32_t width{ 1920 };
        uint32_t height{ 1080 };
        std::string title{ "Yggdrasil CEngine" };
    };

    class CWindow
    {
    public:
        void Create(const SWindowCreateInfo& windowCreateInfo) noexcept;
        void Destroy() noexcept;
        void Update() noexcept;
        bool IsClosed() noexcept;
        void SetTitle(const std::string& title) noexcept;

        // TODO: this is bad, should be private
        using HINSTANCE = void*;
        HINSTANCE m_hInstance{};
        using HWND = void*;
        HWND m_hwnd{};

        struct SWindowData
        {
            uint32_t width;
            uint32_t height;
            std::string title;
            bool isClosed;
        } m_data{};
    };
}
