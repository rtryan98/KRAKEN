#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/GraphicsContext.h"
#include "Yggdrasil/RenderEngine/RenderUtil.h"
#include "Yggdrasil/RenderEngine/RenderEngine.h"
#include "Yggdrasil/Common/Engine.h"

#include <Windows.h>
#include <vulkan/vulkan_win32.h>

namespace Ygg
{
    void GraphicsContext::Create(RenderEngineFeatures* pFeatures)
    {
        VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.pEngineName = "Yggdrasil Engine";
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
        VkCheck(vkCreateInstance(&createInfo, nullptr, &this->instance));

#if YGG_USE_ASSERTS
        CreateDebugMessenger(this->instance);
#endif

        this->pDevice = new GraphicsDevice();
        this->pDevice->Create(this, pFeatures->enableAllFeatures);
        this->screen.CreateSurface(this, &Engine::instance->window);
    }

    void GraphicsContext::Destroy()
    {
        this->screen.Destroy();
        if (this->pDevice != nullptr)
        {
            this->pDevice->Destroy();
            delete this->pDevice;
        }
#if YGG_USE_ASSERTS
        DestroyDebugMessenger(this->instance);
#endif
        if (this->instance != nullptr)
        {
            vkDestroyInstance(this->instance, nullptr);
        }
    }
}
