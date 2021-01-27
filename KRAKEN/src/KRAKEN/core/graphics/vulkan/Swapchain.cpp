#include "KRAKEN/core/graphics/vulkan/Swapchain.h"
#include "KRAKEN/core/graphics/vulkan/Util.h"
#include "KRAKEN/core/graphics/vulkan/Globals.h"

namespace kraken::vulkan
{
    void Swapchain::getSwapchainImages(uint32_t imageCount, const Device& device)
    {
        uint32_t swapchainImageCount{ imageCount };
        this->swapchainImages.resize(swapchainImageCount);
        VK_CHECK(vkGetSwapchainImagesKHR(device.getDevice(), this->swapchain, &swapchainImageCount, this->swapchainImages.data()));
    }

    void Swapchain::createImageViews(const Device& device)
    {
        this->swapchainImageViews.resize(this->swapchainImages.size());

        VkImageViewCreateInfo imageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = this->format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        for (uint32_t i{ 0 }; i < this->swapchainImages.size(); i++)
        {
            imageViewCreateInfo.image = this->swapchainImages[i];
            VK_CHECK(vkCreateImageView(device.getDevice(), &imageViewCreateInfo, VK_CPU_ALLOCATOR, &this->swapchainImageViews[i]));
        }
    }

    void Swapchain::init(VkSurfaceKHR surface, const Window& window, const Device& device)
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.getPhysicalDevice(), surface, &surfaceCapabilities);

        VkSwapchainCreateInfoKHR swapchainCreateInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainCreateInfo.surface = surface;

        if (surfaceCapabilities.minImageCount <= 2 && surfaceCapabilities.maxImageCount >= 2)
        {
            swapchainCreateInfo.minImageCount = 2;
        }
        else
        {
            KRAKEN_CORE_CRITICAL("Double buffering not available.");
        }

        // TODO: add support for HDR and other displays here
        swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapchainCreateInfo.imageExtent = window.getFramebufferSize();

        // TODO: we might want to write directly to the image later on
        if (surfaceCapabilities.supportedUsageFlags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        else
        {
            KRAKEN_CORE_CRITICAL("Swapchain does not support being written to.");
        }

        if (device.getPresentQueueIndex() != device.getGraphicsQueueIndex())
        {
            uint32_t queueFamilyIndices[2]{ device.getPresentQueueIndex(), device.getGraphicsQueueIndex() };
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        uint32_t presentModeCount{ 0 };
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.getPhysicalDevice(), surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.getPhysicalDevice(), surface, &presentModeCount, presentModes.data());

        swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.imageArrayLayers = 1;

        uint32_t surfaceFormatCount{ 0 };
        vkGetPhysicalDeviceSurfaceFormatsKHR(device.getPhysicalDevice(), surface, &surfaceFormatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> availableFormats(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device.getPhysicalDevice(), surface, &surfaceFormatCount, availableFormats.data());

        bool bgraFound{ false };
        bool rgbaFound{ false };
        for (uint32_t i{ 0 }; i < surfaceFormatCount; i++)
        {
            if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                bgraFound = true;
            }
            else if (availableFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM)
            {
                rgbaFound = true;
            }
        }

        if (bgraFound)
        {
            swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        }
        else if (rgbaFound)
        {
            swapchainCreateInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
        }
        else
        {
            KRAKEN_CORE_CRITICAL("No viable surface format found");
        }

        this->format = swapchainCreateInfo.imageFormat;

        VK_CHECK(vkCreateSwapchainKHR(device.getDevice(), &swapchainCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->swapchain));
        KRAKEN_ASSERT_VALUE(this->swapchain);

        getSwapchainImages(swapchainCreateInfo.minImageCount, device);
        createImageViews(device);
    }

    void Swapchain::free(const Device& device)
    {
        for (uint32_t i{ 0 }; i < this->swapchainImageViews.size(); i++)
        {
            vkDestroyImageView(device.getDevice(), this->swapchainImageViews[i], VK_CPU_ALLOCATOR);
        }
        vkDestroySwapchainKHR(device.getDevice(), this->swapchain, vulkan::VK_CPU_ALLOCATOR);
    }

    VkSwapchainKHR Swapchain::getSwapchain() const
    {
        return this->swapchain;
    }
}
