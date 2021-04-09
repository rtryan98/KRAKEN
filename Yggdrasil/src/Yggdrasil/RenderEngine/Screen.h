#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace Ygg
{
    class CGraphicsContext;
    class CWindow;

    class CScreen
    {
    private:
        struct SData;
    public:

        void CreateSurface(CGraphicsContext* const pNewContext, const CWindow* pWindow);
        void CreateSwapchain();
        void Destroy();
        const SData& GetData() const;

    private:
        void CreateRenderPass();
        void CreateFramebuffer();

        struct SData
        {
            VkSurfaceKHR surface;
            VkSwapchainKHR swapchain;
            VkExtent2D swapchainImageExtent;
            VkFormat swapchainImageFormat;
            uint32_t swapchainImageCount;
            std::vector<VkImage> swapchainImages;
            VkFramebuffer swapchainFramebuffer;
            VkRenderPass swapchainRenderPass;
        } m_data{};

        /// @brief non-owned GraphicsContext
        CGraphicsContext* a_pContext{};

        /// @brief non-owned Window
        const CWindow* a_pWindow{};
    };
}
