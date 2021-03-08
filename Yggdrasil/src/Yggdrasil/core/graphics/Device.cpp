#include "Yggdrasil/pch.h"
#include "Yggdrasil/Types.h"
#include "Yggdrasil/core/graphics/Device.h"
#include "Yggdrasil/core/graphics/Util.h"
#include "Yggdrasil/core/util/Log.h"
#include "Yggdrasil/core/graphics/Globals.h"

#include <vector>

namespace yggdrasil::graphics
{
    void selectPhysicalDevice(VkInstance instance, Device* device)
    {
        uint32_t deviceCount{ 0 };
        VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data()));
        YGGDRASIL_ASSERT_VALUE(physicalDevices.size() > 0);

        VkPhysicalDevice fallbackDevice{ physicalDevices[0] };
        VkPhysicalDevice selectedDevice{ fallbackDevice };
        for (uint32_t i{ 0 }; i < physicalDevices.size(); i++)
        {
            VkPhysicalDeviceProperties deviceProperties{};
            vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                selectedDevice = physicalDevices[i];
            }
        }
        device->physical = selectedDevice;
        YGGDRASIL_ASSERT_VALUE_MSG(device->physical != VK_NULL_HANDLE, "No device selected.");
    }

    bool_t queueFamilySupportsGraphicsAndCompute(const VkQueueFamilyProperties& properties)
    {
        return (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (properties.queueFlags & VK_QUEUE_COMPUTE_BIT);
    }

    bool_t queueFamilySupportsPresentation(Device* device, uint32_t queueFamilyIndex, Screen& screen)
    {
        VkBool32 presentationSupported{};
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device->physical, queueFamilyIndex, screen.surface, &presentationSupported));
        return presentationSupported;
    }

    void Device::create(VkInstance instance, Screen& screen)
    {
        selectPhysicalDevice(instance, this);

        uint32_t queueFamilyCount{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties(this->physical, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(this->physical, &queueFamilyCount, queueFamilies.data());

        bool_t transferQueueFamilyFound{ false };
        bool_t asyncComputeQueueFamilyFound{ false };
        bool_t rasterizerQueueFamilyFound{ false };
        bool_t presentQueueFamilyFound{ false };

        for (uint32_t i{ 0 }; i < queueFamilies.size(); i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
                queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT &&
                queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT &&
                !rasterizerQueueFamilyFound)
            {
                this->queues.rasterizerQueueFamilyIndex = i;
                rasterizerQueueFamilyFound = true;
                if (!presentQueueFamilyFound &&
                    queueFamilySupportsPresentation(this, i, screen))
                {
                    presentQueueFamilyFound = true;
                    this->queues.presentQueueFamilyIndex = i;
                }
                else if (queueFamilySupportsPresentation(this, i, screen))
                {
                    this->queues.presentQueueFamilyIndex = i;
                }

                YGGDRASIL_CORE_TRACE("Found main rasterizer queue family with index {0}.", i);
            }
            else if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT &&
                queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT &&
                !asyncComputeQueueFamilyFound)
            {
                this->queues.asyncComputeQueueFamilyIndex = i;
                asyncComputeQueueFamilyFound = true;

                YGGDRASIL_CORE_TRACE("Found async compute queue family with index {0}.", i);
            }
            else if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT &&
                !transferQueueFamilyFound)
            {
                this->queues.transferQueueFamilyIndex = i;
                transferQueueFamilyFound = true;

                YGGDRASIL_CORE_TRACE("Found transfer queue family with index {0}.", i);
            }
        }

        YGGDRASIL_ASSERT_VALUE_MSG(rasterizerQueueFamilyFound, "No main queue family found. Aborting.");
    
        std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos{};
    
        float_t queuePriorities[]{ 1.0f, 1.0f };
        if (asyncComputeQueueFamilyFound)
        {
            VkDeviceQueueCreateInfo asyncComputeQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            asyncComputeQueueCreateInfo.pQueuePriorities = queuePriorities;
            asyncComputeQueueCreateInfo.queueFamilyIndex = this->queues.asyncComputeQueueFamilyIndex;
            asyncComputeQueueCreateInfo.queueCount = 1;
            deviceQueueCreateInfos.push_back(asyncComputeQueueCreateInfo);
        }
        if (transferQueueFamilyFound)
        {
            VkDeviceQueueCreateInfo transferQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            transferQueueCreateInfo.pQueuePriorities = queuePriorities;
            transferQueueCreateInfo.queueFamilyIndex = this->queues.transferQueueFamilyIndex;
            transferQueueCreateInfo.queueCount = 1;
            deviceQueueCreateInfos.push_back(transferQueueCreateInfo);
        }
        VkDeviceQueueCreateInfo mainQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        mainQueueCreateInfo.pQueuePriorities = queuePriorities;
        mainQueueCreateInfo.queueFamilyIndex = this->queues.rasterizerQueueFamilyIndex;
        mainQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfos.push_back(mainQueueCreateInfo);
    
        VkPhysicalDeviceFeatures deviceFeatures{};
    
        // VkPhysicalDeviceVulkan12Features deviceFeaturesVulkan12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
    
        // TODO: this should be checked, especially when optional extensions like VK_KHR_raytracing are wanted
        std::vector<const char*> enabledDeviceExtensions{};
        enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    
        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();
    
        VK_CHECK(vkCreateDevice(this->physical, &deviceCreateInfo, graphics::VK_CPU_ALLOCATOR, &this->logical));
        YGGDRASIL_ASSERT_VALUE(this->logical);
    
        vkGetDeviceQueue(this->logical, this->queues.rasterizerQueueFamilyIndex, 0, &this->queues.rasterizerQueue);
        if (!asyncComputeQueueFamilyFound)
        {
            this->queues.asyncComputeQueue = this->queues.rasterizerQueue;
        }
        else
        {
             vkGetDeviceQueue(this->logical, this->queues.asyncComputeQueueFamilyIndex, 0, &this->queues.asyncComputeQueue);
        }
        if (!transferQueueFamilyFound)
        {
            this->queues.transferQueue = this->queues.rasterizerQueue;
        }
        else
        {
            vkGetDeviceQueue(this->logical, this->queues.transferQueueFamilyIndex, 0, &this->queues.transferQueue);
        }
        if (presentQueueFamilyFound)
        {
            this->queues.presentQueue = this->queues.rasterizerQueue;
        }
        else
        {
            YGGDRASIL_CORE_ERROR("Presentation not supported.");
        }
    
        YGGDRASIL_ASSERT_VALUE(this->queues.rasterizerQueue);
        YGGDRASIL_ASSERT_VALUE(this->queues.presentQueue);
    }

    void Device::free()
    {
        if (this->logical != VK_NULL_HANDLE)
        {
            vkDestroyDevice(this->logical, graphics::VK_CPU_ALLOCATOR);
        }
    }
}
