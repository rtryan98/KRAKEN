#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"
#include "Yggdrasil/RenderEngine/RenderUtil.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"
#include "Yggdrasil/Common/Engine.h"

#include <Windows.h>
#include <vulkan/vulkan_win32.h>

namespace Ygg
{
    void CGraphicsContext::Create()
    {
        VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.pEngineName = "Yggdrasil CEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pApplicationName = "Yggdrasil Application";

        VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        createInfo.pApplicationInfo = &appInfo;

        std::vector<const char*> extensions{};
        extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

#if YGG_USE_ASSERTS
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        std::vector<const char*> validationLayers{};
        validationLayers.push_back("VK_LAYER_KHRONOS_validation");
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
#else
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
#endif
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        RenderUtil::VkCheck(vkCreateInstance(&createInfo, nullptr, &this->m_instance));

#if YGG_USE_ASSERTS
        RenderUtil::CreateDebugMessenger(this->m_instance);
        RenderUtil::InitDebugExtensions(this->m_instance);
#endif

        this->m_screen.CreateSurface(this, &CEngine::GetWindow());
        this->m_device.Create(this);
        this->m_screen.CreateSwapchain();
    }

    void CGraphicsContext::Destroy()
    {
        this->m_device.WaitIdle();
        this->m_screen.Destroy();
        this->m_device.Destroy();
#if YGG_USE_ASSERTS
        RenderUtil::DestroyDebugMessenger(this->m_instance);
#endif
        if (this->m_instance != nullptr)
        {
            vkDestroyInstance(this->m_instance, nullptr);
        }
    }

    const CScreen& CGraphicsContext::GetScreen() const
    {
        return this->m_screen;
    }

    VkInstance CGraphicsContext::GetVkInstance() const
    {
        return this->m_instance;
    }

    const CGraphicsDevice& CGraphicsContext::GetGraphicsDevice() const
    {
        return this->m_device;
    }

    CGraphicsDevice& CGraphicsContext::GetGraphicsDeviceNonConst()
    {
        return this->m_device;
    }
}
