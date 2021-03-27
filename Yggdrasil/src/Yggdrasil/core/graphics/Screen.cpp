#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/Screen.h"
#include "Yggdrasil/core/graphics/Globals.h"
#include "Yggdrasil/core/graphics/Device.h"
#include "Yggdrasil/core/graphics/Util.h"

#include <GLFW/glfw3.h>

namespace ygg::graphics
{
    void Screen::createSurface(Window* win, VkInstance instance)
    {
        YGGDRASIL_CORE_TRACE("Creating Surface.");
        this->window = win;
        this->surface = window->getSurface(instance);
    }

    void getImages(Screen* screen, Device& device)
    {
        YGGDRASIL_CORE_TRACE("Acquiring Swapchain Images.");

        uint32_t swapchainImageCount{};
        VK_CHECK(vkGetSwapchainImagesKHR(device.logical, screen->swapchain, &swapchainImageCount, nullptr));
        screen->swapchainImages.resize(swapchainImageCount);
        VK_CHECK(vkGetSwapchainImagesKHR(device.logical, screen->swapchain, &swapchainImageCount, screen->swapchainImages.data()));
    }

    void createImageViews(Screen* screen, Device& device)
    {
        YGGDRASIL_CORE_TRACE("Creating Swapchain Image Views.");

        screen->swapchainImageViews.resize(screen->swapchainImages.size());

        VkImageViewCreateInfo imageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = screen->swapchainImageFormat;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        for (uint32_t i{ 0 }; i < screen->swapchainImages.size(); i++)
        {
            imageViewCreateInfo.image = screen->swapchainImages[i];
            VK_CHECK(vkCreateImageView(device.logical, &imageViewCreateInfo, graphics::VK_CPU_ALLOCATOR, &screen->swapchainImageViews[i]));
        }
    }

    void Screen::createSwapchain(Device& device)
    {
        YGGDRASIL_CORE_TRACE("Creating Swapchain.");
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physical, this->surface, &surfaceCapabilities);

        VkSwapchainCreateInfoKHR swapchainCreateInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainCreateInfo.surface = this->surface;

        if (surfaceCapabilities.minImageCount <= 2 && surfaceCapabilities.maxImageCount >= 2)
        {
            swapchainCreateInfo.minImageCount = 2;
        }
        else
        {
            YGGDRASIL_CORE_CRITICAL("Double buffering not available.");
        }

        // TODO: add support for HDR and other displays here
        swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapchainCreateInfo.imageExtent = window->getFramebufferSize();
        this->swapchainImageExtent = swapchainCreateInfo.imageExtent;

        // TODO: we might want to write directly to the image later on
        if (surfaceCapabilities.supportedUsageFlags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        else
        {
            YGGDRASIL_CORE_CRITICAL("Swapchain does not support being written to.");
        }

        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        uint32_t presentModeCount{ 0 };
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.physical, this->surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.physical, this->surface, &presentModeCount, presentModes.data());

        swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.imageArrayLayers = 1;

        uint32_t surfaceFormatCount{ 0 };
        vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical, this->surface, &surfaceFormatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> availableFormats(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical, this->surface, &surfaceFormatCount, availableFormats.data());

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
            YGGDRASIL_CORE_CRITICAL("No viable surface format found");
        }

        this->swapchainImageFormat = swapchainCreateInfo.imageFormat;

        VK_CHECK(vkCreateSwapchainKHR(device.logical, &swapchainCreateInfo, graphics::VK_CPU_ALLOCATOR, &this->swapchain));
        YGGDRASIL_ASSERT_VALUE(this->swapchain);

        getImages(this, device);
        createImageViews(this, device);
        createSwapchainRenderPass(device);
        createSwapchainFramebuffers(device);
    }

    void Screen::createSwapchainRenderPass(Device& device)
    {
        YGGDRASIL_CORE_TRACE("Creating Swapchain Render Pass.");

        VkAttachmentDescription attachmentDescription{};
        attachmentDescription.format = this->swapchainImageFormat;
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentReference{};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;

        VkRenderPassCreateInfo renderPassCreateInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &attachmentDescription;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 0;
        renderPassCreateInfo.pDependencies = nullptr;

        VK_CHECK(vkCreateRenderPass(device.logical, &renderPassCreateInfo, graphics::VK_CPU_ALLOCATOR, &this->swapchainRenderPass));
        VK_SET_OBJECT_DEBUG_NAME(device, reinterpret_cast<uint64_t>(this->swapchainRenderPass), VK_OBJECT_TYPE_RENDER_PASS, "Swapchain Renderpass");
    }

    void Screen::createSwapchainFramebuffers(Device& device)
    {
        YGGDRASIL_CORE_TRACE("Creating Swapchain Framebuffers.");

        this->swapchainFramebuffers.resize(this->swapchainImageViews.size());

        VkFramebufferCreateInfo framebufferCreateInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        framebufferCreateInfo.renderPass = this->swapchainRenderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.width = this->swapchainImageExtent.width;
        framebufferCreateInfo.height = this->swapchainImageExtent.height;
        framebufferCreateInfo.layers = 1;

        for (uint32_t i{ 0 }; i < this->swapchainImageViews.size(); i++)
        {
            framebufferCreateInfo.pAttachments = &this->swapchainImageViews[i];
            VK_CHECK(vkCreateFramebuffer(device.logical, &framebufferCreateInfo, graphics::VK_CPU_ALLOCATOR, &this->swapchainFramebuffers[i]));
            std::string framebufferName{ "Swapchain Framebuffer" + i };
            VK_SET_OBJECT_DEBUG_NAME(device, reinterpret_cast<uint64_t>(this->swapchainFramebuffers[i]), VK_OBJECT_TYPE_FRAMEBUFFER, framebufferName.c_str());
        }
    }

    void Screen::freeSwapchainFramebuffers(Device& device)
    {
        for (uint32_t i{ 0 }; i < this->swapchainFramebuffers.size(); i++)
        {
            if (this->swapchainFramebuffers[i] != VK_NULL_HANDLE)
            {
                YGGDRASIL_CORE_TRACE("Destroying Swapchain Framebuffer '{0}'.", i);
                vkDestroyFramebuffer(device.logical, this->swapchainFramebuffers[i], graphics::VK_CPU_ALLOCATOR);
            }
        }
    }

    void Screen::freeSwapchain(Device& device)
    {
        if (this->swapchainRenderPass != VK_NULL_HANDLE)
        {
            YGGDRASIL_CORE_TRACE("Destroying Swapchain Renderpass.");
            vkDestroyRenderPass(device.logical, this->swapchainRenderPass, graphics::VK_CPU_ALLOCATOR);
        }
        for (uint32_t i{ 0 }; i < this->swapchainImageViews.size(); i++)
        {
            if (this->swapchainImageViews[i] != VK_NULL_HANDLE)
            {
                YGGDRASIL_CORE_TRACE("Destroying Swapchain Image View '{0}'.", i);
                vkDestroyImageView(device.logical, this->swapchainImageViews[i], graphics::VK_CPU_ALLOCATOR);
            }
        }
        if (this->swapchain != VK_NULL_HANDLE)
        {
            YGGDRASIL_CORE_TRACE("Destroying Swapchain.");
            vkDestroySwapchainKHR(device.logical, this->swapchain, graphics::VK_CPU_ALLOCATOR);
        }
    }

    void Screen::freeSurface(VkInstance instance)
    {
        if (this->surface != VK_NULL_HANDLE)
        {
            YGGDRASIL_CORE_TRACE("Destroying Surface.");
            vkDestroySurfaceKHR(instance, this->surface, graphics::VK_CPU_ALLOCATOR);
        }
    }

    void Screen::recreateSwapchain(Device& device)
    {
        VkExtent2D extent{ window->getFramebufferSize() };
        int32_t width{ static_cast<int32_t>(extent.width) };
        int32_t height{ static_cast<int32_t>(extent.height) };
        while (width == 0 || height == 0)
        {
            // TODO: don't pause the application.
            // skip rendering instead.
            glfwGetFramebufferSize(window->getNativeWindow(), &width, &height);
            glfwWaitEvents();
        }

        VK_CHECK(vkDeviceWaitIdle(device.logical));

        freeSwapchainFramebuffers(device);
        freeSwapchain(device);
        createSwapchain(device);
        // createSwapchainRenderPass(device);
        // createSwapchainFramebuffers(device);
    }
}
