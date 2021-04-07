#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace Ygg
{
    struct GraphicsContext;
    class Window;

    struct Screen
    {
        VkSurfaceKHR surface;
        VkSwapchainKHR swapchain;
        VkExtent2D swapchainImageExtent;
        VkFormat swapchainImageFormat;
        uint32_t swapchainImageCount;
        std::vector<VkImage> swapchainImages;

        void CreateSurface(GraphicsContext* const pNewContext, Window* window);
        void CreateSwapchain();
        void Destroy();

    private:
        GraphicsContext* pContext;
        Window* pWindow;
    };
}
