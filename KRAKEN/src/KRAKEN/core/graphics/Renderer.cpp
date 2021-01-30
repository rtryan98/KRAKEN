#include "KRAKEN/core/graphics/Renderer.h"
#include "KRAKEN/core/Globals.h"
#include <vector>
#include "KRAKEN/core/Application.h"
#include <map>
#include "KRAKEN/core/window/Window.h"

namespace kraken
{
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
        )
    {
        KRAKEN_UNUSED_VARIABLE(pUserData);
        KRAKEN_UNUSED_VARIABLE(type);
        if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            ::kraken::Logger::getValidationErrorLogger()->error("{0}", pCallbackData->pMessage);
            debugBreak();
        }
        else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            ::kraken::Logger::getValidationErrorLogger()->warn("{0}", pCallbackData->pMessage);
        }
        else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            ::kraken::Logger::getValidationErrorLogger()->trace("{0}", pCallbackData->pMessage);
        }
        return VK_FALSE;
    }

    void Renderer::createInstance()
    {
        VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.pApplicationName = constants::ENGINE_NAME;
        appInfo.applicationVersion = VK_MAKE_VERSION(constants::VERSION_MAJOR, constants::VERSION_MINOR, constants::VERSION_PATCH);
        appInfo.pEngineName = constants::ENGINE_NAME;
        appInfo.engineVersion = VK_MAKE_VERSION(constants::VERSION_MAJOR, constants::VERSION_MINOR, constants::VERSION_PATCH);

        VkInstanceCreateInfo instanceCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instanceCreateInfo.pApplicationInfo = &appInfo;

        std::vector<const char*> platformExtensions{};
        const char** platformExtensionsCstr{};
        uint32_t platformExtensionCount{ 0 };
        globals::APPLICATION->getWindow()->getRequiredPlatformExtensions(&platformExtensionCount, &platformExtensionsCstr);
        for (uint32_t i{ 0 }; i < platformExtensionCount; i++)
        {
            platformExtensions.emplace_back(platformExtensionsCstr[i]);
        }
        platformExtensions.emplace_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(platformExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = platformExtensions.data();

#if KRAKEN_USE_ASSERTS
        std::vector<const char*> requiredValidationLayers {
            "VK_LAYER_KHRONOS_validation"
        };

        uint32_t availableInstanceLayerCount{ 0 };
        VK_CHECK(vkEnumerateInstanceLayerProperties(&availableInstanceLayerCount, nullptr));
        std::vector<VkLayerProperties> availableLayers(availableInstanceLayerCount);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&availableInstanceLayerCount, availableLayers.data()));

        for (uint32_t i{ 0 }; i < requiredValidationLayers.size(); i++)
        {
            bool_t found{ 0 };
            for (uint32_t j{ 0 }; j < availableInstanceLayerCount; j++)
            {
                if ( strcmp(requiredValidationLayers[i], availableLayers[j].layerName) == 0 )
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                KRAKEN_CORE_CRITICAL("Validation Layer unavailable: {0}", requiredValidationLayers[i]);
            }
        }

        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
#else
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.ppEnabledLayerNames = nullptr;
#endif
        VK_CHECK(vkCreateInstance(&instanceCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->instance));
        KRAKEN_ASSERT_VALUE(this->instance);
    }

    void Renderer::setupDebugMessenger()
    {
        vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(this->instance, "vkCreateDebugUtilsMessengerEXT"));
        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(this->instance, "vkDestroyDebugUtilsMessengerEXT"));

        VkDebugUtilsMessengerCreateInfoEXT createInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;

        VK_CHECK(vkCreateDebugUtilsMessengerEXT(this->instance, &createInfo, vulkan::VK_CPU_ALLOCATOR, &this->debugMessenger));
        KRAKEN_ASSERT_VALUE(this->debugMessenger);
    }

    void Renderer::createSurface()
    {
        this->surface = globals::APPLICATION->getWindow()->getSurface(this->instance);
    }

    void Renderer::createCommandPool()
    {
        VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        createInfo.queueFamilyIndex = this->device.getGraphicsQueueIndex();
        VK_CHECK(vkCreateCommandPool(this->device.getDevice(), &createInfo, vulkan::VK_CPU_ALLOCATOR, &this->commandPool));
    }

    void Renderer::onUpdate()
    {
        vkDeviceWaitIdle(this->device.getDevice());
        uint32_t imageIndex{};
        VK_CHECK(vkAcquireNextImageKHR(this->device.getDevice(), this->swapchain.getSwapchain(), ~0ull, this->acquireSemaphore, VK_NULL_HANDLE, &imageIndex));

        VK_CHECK(vkResetCommandPool(this->device.getDevice(), this->commandPool, 0x0));

        VkCommandBufferAllocateInfo commandBufferAllocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = this->commandPool;
        commandBufferAllocateInfo.commandBufferCount = 1;

        VK_CHECK(vkAllocateCommandBuffers(this->device.getDevice(), &commandBufferAllocateInfo, &commandBuffer));

        VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        // TODO: TEMPORARY
        VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = this->swapchain.getSwapchainImages()[imageIndex];
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        // TODO: TEMPORARY

        VK_CHECK(vkBeginCommandBuffer(this->commandBuffer, &commandBufferBeginInfo));
        // TODO: TEMPORARY
        vkCmdPipelineBarrier(this->commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        // TODO: TEMPORARY

        // VkClearColorValue clearColor{ 1.0f, 0.5f, 0.0f, 1.0f };
        // VkImageSubresourceRange range{};
        // range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // range.levelCount = 1;
        // range.layerCount = 1;
        // vkCmdClearColorImage(this->commandBuffer, this->swapchain.getSwapchainImages()[imageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &range);
        VK_CHECK(vkEndCommandBuffer(this->commandBuffer));

        VkPipelineStageFlags submitStageMask{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &this->acquireSemaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &this->releaseSemaphore;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &this->commandBuffer;
        submitInfo.pWaitDstStageMask = &submitStageMask;

        VK_CHECK(vkQueueSubmit(this->device.getGraphicsComputeQueue(), 1, &submitInfo, VK_NULL_HANDLE));

        VkPresentInfoKHR queuePresentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        queuePresentInfo.swapchainCount = 1;
        queuePresentInfo.pSwapchains = this->swapchain.getSwapchainAddress();
        queuePresentInfo.waitSemaphoreCount = 1;
        queuePresentInfo.pWaitSemaphores = &this->releaseSemaphore;
        queuePresentInfo.pImageIndices = &imageIndex;

        VK_CHECK(vkQueuePresentKHR(this->device.getGraphicsComputeQueue(), &queuePresentInfo));
    }

    void Renderer::init(const Window& window)
    {
        createInstance();
#if KRAKEN_USE_ASSERTS
        setupDebugMessenger();
#endif
        createSurface();
        device.init(this->instance, this->surface);
        swapchain.init(this->surface, window, this->device);
        this->acquireSemaphore = vulkan::util::createSemaphore(this->device.getDevice());
        this->releaseSemaphore = vulkan::util::createSemaphore(this->device.getDevice());
        this->submitFence = vulkan::util::createFence(this->device.getDevice());
        createCommandPool();
    }

    void Renderer::free()
    {
        VK_CHECK(vkDeviceWaitIdle(this->device.getDevice()));
        vkDestroyCommandPool(this->device.getDevice(), this->commandPool, vulkan::VK_CPU_ALLOCATOR);
        vkDestroyFence(this->device.getDevice(), this->submitFence, vulkan::VK_CPU_ALLOCATOR);
        vkDestroySemaphore(this->device.getDevice(), this->releaseSemaphore, vulkan::VK_CPU_ALLOCATOR);
        vkDestroySemaphore(this->device.getDevice(), this->acquireSemaphore, vulkan::VK_CPU_ALLOCATOR);
        swapchain.free(this->device);
        device.free();
        vkDestroySurfaceKHR(this->instance, this->surface, vulkan::VK_CPU_ALLOCATOR);
#if KRAKEN_USE_ASSERTS
        vkDestroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, vulkan::VK_CPU_ALLOCATOR);
#endif
        vkDestroyInstance(this->instance, vulkan::VK_CPU_ALLOCATOR);
    }

    VkInstance Renderer::getInstance() const
    {
        return this->instance;
    }
}
