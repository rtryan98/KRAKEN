#include "KRAKEN/core/graphics/vulkan/Swapchain.h"
#include "KRAKEN/core/graphics/vulkan/Util.h"
#include "KRAKEN/core/graphics/vulkan/Globals.h"

namespace kraken::vulkan
{
    void Swapchain::init(VkSurfaceKHR surface, const Window& window, const Device& device)
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.getPhysicalDevice(), surface, &surfaceCapabilities);

        VkSwapchainCreateInfoKHR swapchainCreateInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainCreateInfo.surface = surface;

        // Triple buffer if possible, double buffer elsewise, otherwise single buffer
        if (surfaceCapabilities.minImageCount >= 3)
        {
            swapchainCreateInfo.minImageCount = 3;
        }
        else if (surfaceCapabilities.minImageCount == 2)
        {
            swapchainCreateInfo.minImageCount = 2;
        }
        else
        {
            swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount;
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

        bool_t mailboxAvailable{ false };
        for (VkPresentModeKHR presentMode : presentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                mailboxAvailable = true;
                break;
            }
        }

        if (mailboxAvailable)
        {
            swapchainCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        }
        else
        {
            swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        }

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

        VK_CHECK(vkCreateSwapchainKHR(device.getDevice(), &swapchainCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->swapchain));
        KRAKEN_ASSERT_VALUE(this->swapchain);

        uint32_t swapchainImageCount{ swapchainCreateInfo.minImageCount };
        VK_CHECK(vkGetSwapchainImagesKHR(device.getDevice(), this->swapchain, &swapchainImageCount, nullptr));
        this->swapchainImages.reserve(swapchainImageCount);
        VK_CHECK(vkGetSwapchainImagesKHR(device.getDevice(), this->swapchain, &swapchainImageCount, this->swapchainImages.data()));
    }

    void Swapchain::free(const Device& device)
    {
        vkDestroySwapchainKHR(device.getDevice(), this->swapchain, vulkan::VK_CPU_ALLOCATOR);
    }

    VkSwapchainKHR Swapchain::getSwapchain() const
    {
        return this->swapchain;
    }
}
