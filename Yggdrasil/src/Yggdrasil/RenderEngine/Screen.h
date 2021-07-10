// Copyright 2021 Robert Ryan. See LICENCE.md.

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
        void BeginSwapchainRenderPass(VkCommandBuffer cmdBuffer, uint32_t swapchainImageIndex) const;
        const SData& GetData() const;

    private:
        void CreateRenderPass();
        void CreateImageViews();
        void CreateFramebuffer();
        VkPresentModeKHR ChosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

        struct SData
        {
            VkSurfaceKHR surface;
            VkSwapchainKHR swapchain;
            VkExtent2D swapchainImageExtent;
            VkFormat swapchainImageFormat;
            uint32_t swapchainImageCount;
            std::vector<VkImage> swapchainImages;
            std::vector<VkImageView> swapchainImageViews;
            VkFramebuffer swapchainFramebuffer;
            VkRenderPass swapchainRenderPass;
        } m_data{};

        /// @brief non-owned CGraphicsContext
        CGraphicsContext* a_pContext{};

        /// @brief non-owned CWindow
        const CWindow* a_pWindow{};
    };
}
