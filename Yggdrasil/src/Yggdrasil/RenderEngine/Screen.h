#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace Ygg
{
    class GraphicsContext;
    class Window;

    class Screen
    {
    private:
        struct Data;
    public:

        void CreateSurface(GraphicsContext* const pNewContext, Window* window);
        void CreateSwapchain();
        void Destroy();
        Data& GetData();

    private:
        void CreateRenderPass();
        void CreateFramebuffer();

        struct Data
        {
            VkSurfaceKHR surface;
            VkSwapchainKHR swapchain;
            VkExtent2D swapchainImageExtent;
            VkFormat swapchainImageFormat;
            uint32_t swapchainImageCount;
            std::vector<VkImage> swapchainImages;
            VkFramebuffer swapchainFramebuffer;
            VkRenderPass swapchainRenderPass;
        } data{};

        /// @brief non-owned GraphicsContext
        GraphicsContext* pContext{};

        /// @brief non-owned Window
        Window* pWindow{};
    };
}
