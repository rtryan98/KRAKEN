#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/Screen.h"
#include "Yggdrasil/RenderEngine/RenderUtil.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"
#include "Yggdrasil/Common/Window/Window.h"
#include <Windows.h>
#include <vulkan/vulkan_win32.h>

namespace Ygg
{
    void Screen::CreateSurface(GraphicsContext* const pNewContext, Window* pNewWindow)
    {
        this->pContext = pNewContext;
        this->pWindow = pNewWindow;

        VkWin32SurfaceCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
        createInfo.hinstance = GetModuleHandle(nullptr);
        createInfo.hwnd = static_cast<::HWND>(this->pWindow->hwnd);

        VkCheck( vkCreateWin32SurfaceKHR(this->pContext->instance, &createInfo, nullptr, &this->surface) );
    }

    void Screen::CreateSwapchain()
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{ this->pContext->pDevice->gpu.GetSurfaceCapabilitiesKHR(this->surface) };

        VkSwapchainCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        createInfo.surface = this->surface;
        if (surfaceCapabilities.minImageCount <= 2 && surfaceCapabilities.maxImageCount >= 2)
        {
            createInfo.minImageCount = 2;
        }
        else
        {
            YGG_CRITICAL("Double buffering not available!");
        }

        // TODO: add support for HDR and other displays here
        createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        createInfo.imageExtent = { this->pWindow->data.width, this->pWindow->data.height };
        this->swapchainImageExtent = createInfo.imageExtent;

        // TODO: we might want to write directly to the image later on
        if (surfaceCapabilities.supportedUsageFlags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            createInfo.imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        else
        {
            YGG_CRITICAL("Swapchain does not support being written to!");
        }

        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        uint32_t presentModeCount{ 0 };
        this->pContext->pDevice->gpu.GetSurfacePresentModesKHR(this->surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        this->pContext->pDevice->gpu.GetSurfacePresentModesKHR(this->surface, &presentModeCount, presentModes.data());

        createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        createInfo.clipped = VK_TRUE;
        createInfo.imageArrayLayers = 1;

        uint32_t surfaceFormatCount{ 0 };
        this->pContext->pDevice->gpu.GetSurfaceFormatsKHR(this->surface, &surfaceFormatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> availableFormats(surfaceFormatCount);
        this->pContext->pDevice->gpu.GetSurfaceFormatsKHR(this->surface, &surfaceFormatCount, availableFormats.data());

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
            createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        }
        else if (rgbaFound)
        {
            createInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
        }
        else
        {
            YGG_CRITICAL("No viable surface format found!");
        }

        this->swapchainImageFormat = createInfo.imageFormat;
        this->swapchain = this->pContext->pDevice->CreateSwapchainKHR(&createInfo);
        VkCheck( vkGetSwapchainImagesKHR(this->pContext->pDevice->handle, this->swapchain, &this->swapchainImageCount, nullptr) );
        this->swapchainImages.resize(this->swapchainImageCount);
        VkCheck( vkGetSwapchainImagesKHR(this->pContext->pDevice->handle, this->swapchain, &this->swapchainImageCount, this->swapchainImages.data()) );

        CreateRenderPass();
        CreateFramebuffer();
    }

    void Screen::Destroy()
    {
        this->pContext->pDevice->DestroyFramebuffer(&this->swapchainFramebuffer);
        this->pContext->pDevice->DestroyRenderPass(&this->swapchainRenderPass);
        this->pContext->pDevice->DestroySwapchainKHR(&this->swapchain);
        if (this->surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(this->pContext->instance, this->surface, nullptr);
        }
    }

    void Screen::CreateRenderPass()
    {
        VkAttachmentDescription attachment{};
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachment.format = this->swapchainImageFormat;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;

        VkAttachmentReference attachmentRef{};
        attachmentRef.attachment = 0;
        attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &attachmentRef;

        VkRenderPassCreateInfo createInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &attachment;
        createInfo.dependencyCount = 0;
        createInfo.pDependencies = nullptr;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpass;

        this->swapchainRenderPass = this->pContext->pDevice->CreateRenderPass(&createInfo);
    }

    void Screen::CreateFramebuffer()
    {
        VkFramebufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        createInfo.renderPass = this->swapchainRenderPass;
        createInfo.width = this->swapchainImageExtent.width;
        createInfo.height = this->swapchainImageExtent.height;
        createInfo.layers = 1;
        createInfo.flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;

        this->swapchainFramebuffer = this->pContext->pDevice->CreateFramebuffer(&createInfo);
    }
}
