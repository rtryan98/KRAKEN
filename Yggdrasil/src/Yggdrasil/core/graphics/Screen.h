#pragma once
#include "Yggdrasil/core/window/Window.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace ygg::graphics
{
    class Device;

    class Screen
    {
    public:
        void createSurface(Window* win, VkInstance instance);
        void freeSurface(VkInstance instance);
        void createSwapchain(Device& device);
        void freeSwapchain(Device& device);
        void createSwapchainRenderPass(Device& device);
        void createSwapchainFramebuffers(Device& device);
        void freeSwapchainFramebuffers(Device& device);

        void recreateSwapchain(Device& device);

        VkSurfaceKHR surface{};
        VkSwapchainKHR swapchain{};
        VkFormat swapchainImageFormat{};
        VkExtent2D swapchainImageExtent{};
        VkRenderPass swapchainRenderPass{};
        std::vector<VkImage> swapchainImages{};
        std::vector<VkImageView> swapchainImageViews{};
        std::vector<VkFramebuffer> swapchainFramebuffers{};
    private:
        Window* window{ nullptr };
    };
}
