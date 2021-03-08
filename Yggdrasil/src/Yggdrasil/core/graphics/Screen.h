#pragma once
#include "Yggdrasil/core/window/Window.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace yggdrasil::graphics
{
    class Device;

    class Screen
    {
    public:
        void createSurface(Window* win, VkInstance instance);
        void freeSurface(VkInstance instance);
        void createSwapchain(Device& device);
        void freeSwapchain(Device& device);
        void createSwapchainFramebuffers(Device& device, VkRenderPass renderPass);
        void freeSwapchainFramebuffers(Device& device);

        void recreateSwapchain(Device& device, VkRenderPass renderPass);

        VkSurfaceKHR surface{};
        VkSwapchainKHR swapchain{};
        VkFormat swapchainImageFormat{};
        VkExtent2D swapchainImageExtent{};
        std::vector<VkImage> swapchainImages{};
        std::vector<VkImageView> swapchainImageViews{};
        std::vector<VkFramebuffer> swapchainFramebuffers{};
    private:
        Window* window{ nullptr };
    };
}
