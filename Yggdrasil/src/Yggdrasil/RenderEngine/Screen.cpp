#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/Screen.h"
#include "Yggdrasil/RenderEngine/RenderUtil.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"
#include "Yggdrasil/Common/Window/Window.h"
#include <Windows.h>
#include <vulkan/vulkan_win32.h>

namespace Ygg
{
    void CScreen::CreateSurface(CGraphicsContext* const pNewContext, const CWindow* pWindow)
    {
        this->a_pContext = pNewContext;
        this->a_pWindow = pWindow;

        VkWin32SurfaceCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
        createInfo.hinstance = GetModuleHandle(nullptr);
        createInfo.hwnd = static_cast<::HWND>(this->a_pWindow->m_hwnd);

        RenderUtil::VkCheck( vkCreateWin32SurfaceKHR(this->a_pContext->GetVkInstance(), &createInfo, nullptr,
            &this->m_data.surface) );
    }

    void CScreen::CreateSwapchain()
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{ this->a_pContext->GetGraphicsDevice().GetGPU().GetSurfaceCapabilitiesKHR(this->m_data.surface) };

        VkSwapchainCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        createInfo.surface = this->m_data.surface;
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
        createInfo.imageExtent = { this->a_pWindow->m_data.width, this->a_pWindow->m_data.height };
        this->m_data.swapchainImageExtent = createInfo.imageExtent;

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
        this->a_pContext->GetGraphicsDevice().GetGPU().GetSurfacePresentModesKHR(this->m_data.surface, &presentModeCount,
            nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        this->a_pContext->GetGraphicsDevice().GetGPU().GetSurfacePresentModesKHR(this->m_data.surface, &presentModeCount,
            presentModes.data());

        createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        createInfo.clipped = VK_TRUE;
        createInfo.imageArrayLayers = 1;

        uint32_t surfaceFormatCount{ 0 };
        this->a_pContext->GetGraphicsDevice().GetGPU().GetSurfaceFormatsKHR(this->m_data.surface, &surfaceFormatCount,
            nullptr);
        std::vector<VkSurfaceFormatKHR> availableFormats(surfaceFormatCount);
        this->a_pContext->GetGraphicsDevice().GetGPU().GetSurfaceFormatsKHR(this->m_data.surface, &surfaceFormatCount,
            availableFormats.data());

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

        this->m_data.swapchainImageFormat = createInfo.imageFormat;
        this->m_data.swapchain = this->a_pContext->GetGraphicsDevice().CreateSwapchainKHR(&createInfo);
        RenderUtil::VkCheck( vkGetSwapchainImagesKHR(this->a_pContext->GetGraphicsDeviceNonConst().GetHandle(),
            this->m_data.swapchain, &this->m_data.swapchainImageCount, nullptr) );
        this->m_data.swapchainImages.resize(this->m_data.swapchainImageCount);
        RenderUtil::VkCheck( vkGetSwapchainImagesKHR(this->a_pContext->GetGraphicsDeviceNonConst().GetHandle(),
            this->m_data.swapchain, &this->m_data.swapchainImageCount, this->m_data.swapchainImages.data()) );

        CreateRenderPass();
        CreateFramebuffer();
    }

    void CScreen::Destroy()
    {
        this->a_pContext->GetGraphicsDeviceNonConst().DestroyFramebuffer(&this->m_data.swapchainFramebuffer);
        this->a_pContext->GetGraphicsDeviceNonConst().DestroyRenderPass(&this->m_data.swapchainRenderPass);
        this->a_pContext->GetGraphicsDeviceNonConst().DestroySwapchainKHR(&this->m_data.swapchain);
        if (this->m_data.surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(this->a_pContext->GetVkInstance(), this->m_data.surface, nullptr);
        }
    }

    void CScreen::CreateRenderPass()
    {
        VkAttachmentDescription attachment{};
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachment.format = this->m_data.swapchainImageFormat;
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

        this->m_data.swapchainRenderPass = this->a_pContext->GetGraphicsDevice().CreateRenderPass(&createInfo);
    }

    void CScreen::CreateFramebuffer()
    {
        VkFramebufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        createInfo.renderPass = this->m_data.swapchainRenderPass;
        createInfo.width = this->m_data.swapchainImageExtent.width;
        createInfo.height = this->m_data.swapchainImageExtent.height;
        createInfo.layers = 1;
        createInfo.flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;

        this->m_data.swapchainFramebuffer = this->a_pContext->GetGraphicsDevice().CreateFramebuffer(&createInfo);
    }

    const CScreen::SData& CScreen::GetData() const
    {
        return this->m_data;
    }
}
