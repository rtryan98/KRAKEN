#include "KRAKEN/core/graphics/vulkan/Device.h"
#include <vector>
#include "KRAKEN/Defines.h"
#include "KRAKEN/core/graphics/vulkan/Globals.h"
#include "KRAKEN/core/graphics/vulkan/Util.h"
#include "KRAKEN/Types.h"
#include <set>

namespace kraken::vulkan
{
    void Device::free()
    {
        vkDeviceWaitIdle(this->device);
        vkDestroyDevice(this->device, VK_CPU_ALLOCATOR);
    }

    void Device::init(VkInstance instance, VkSurfaceKHR surface)
    {
        selectPhysicalDevice(instance);
        logPhysicalDeviceInfo();
        createDevice(surface);
    }

    void Device::selectPhysicalDevice(VkInstance instance)
    {
        uint32_t deviceCount{ 0 };
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
        KRAKEN_ASSERT_VALUE(physicalDevices.size() > 0);

        VkPhysicalDevice fallbackDevice{ physicalDevices[0] };
        VkPhysicalDevice selectedDevice{ fallbackDevice };
        for (uint32_t i{ 0 }; i < physicalDevices.size(); i++)
        {
            VkPhysicalDeviceProperties deviceProperties{};
            vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
            // VkPhysicalDeviceFeatures   deviceFeatures{};
            // vkGetPhysicalDeviceFeatures(physicalDevices[i], &deviceFeatures);

            // TODO: we pick the first discrete GPU here. This should be able to be chosen at will.
            // perhaps even automate the selection process to chose based on properties
            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                selectedDevice = physicalDevices[i];
            }
        }
        physicalDevice = selectedDevice;
        KRAKEN_ASSERT_VALUE(this->physicalDevice);
    }

    void Device::logPhysicalDeviceInfo()
    {
        VkPhysicalDeviceProperties deviceProperties{};
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);

        KRAKEN_CORE_INFO("Selected Device: {0}", deviceProperties.deviceName);
        KRAKEN_CORE_INFO("Driver Version: {0}", deviceProperties.driverVersion);
        KRAKEN_CORE_INFO("Memory:");
        for (uint32_t i{ 0 }; i < deviceMemoryProperties.memoryHeapCount; i++)
        {
            KRAKEN_CORE_INFO("\tHeap Size: {0:.6f} GB", deviceMemoryProperties.memoryHeaps[i].size * 1.0e-9);
            std::stringstream heapTypeStringStream{};
            heapTypeStringStream << "\t\tHeapType: ";
            for (uint32_t j{ 0 }; j < deviceMemoryProperties.memoryTypeCount; j++)
            {
                if (deviceMemoryProperties.memoryTypes[j].heapIndex == i)
                {
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                    {
                        heapTypeStringStream << "DEVICE_LOCAL\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD)
                    {
                        heapTypeStringStream << "DEVICE_DEVICE_COHERENT_AMD\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD)
                    {
                        heapTypeStringStream << "DEVICE_UNCACHED_AMD\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
                    {
                        heapTypeStringStream << "HOST_VISIBLE\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
                    {
                        heapTypeStringStream << "HOST_COHERENT\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
                    {
                        heapTypeStringStream << "HOST_CACHED\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
                    {
                        heapTypeStringStream << "LAZILY_ALLOCATED\t";
                    }
                    if (deviceMemoryProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT)
                    {
                        heapTypeStringStream << "PROTECTED\t";
                    }
                }
            }
            KRAKEN_CORE_INFO(heapTypeStringStream.str().c_str());
        }
    }

    void Device::createDevice(VkSurfaceKHR surface)
    {
        uint32_t queueFamilyCount{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, queueFamilies.data());

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
        float_t queuePriority{ 1.0f };

        for (uint32_t i{ 0 }; i < queueFamilyCount; i++)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            queueCreateInfo.queueFamilyIndex = i;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            bool_t queueFound{ false };

            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                this->graphicsQueueIndex = i;
                queueCreateInfo.queueCount++;
                queueFound = true;
            }
            if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                this->computeQueueIndex = i;
                queueCreateInfo.queueCount++;
                queueFound = true;
            }
            VkBool32 presentationSupport{ 0 };
            vkGetPhysicalDeviceSurfaceSupportKHR(this->physicalDevice, i, surface, &presentationSupport);
            if (presentationSupport)
            {
                this->presentQueueIndex = i;
                queueCreateInfo.queueCount++;
                queueFound = true;
            }
            if (queueFound)
            {
                queueCreateInfos.push_back(queueCreateInfo);
            }
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        // VkPhysicalDeviceVulkan12Features deviceFeaturesVulkan12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };

        // TODO: this should be checked, especially when optional extensions like VK_KHR_raytracing are wanted
        std::vector<const char*> enabledDeviceExtensions{};
        enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();

        VK_CHECK(vkCreateDevice(this->physicalDevice, &deviceCreateInfo, VK_CPU_ALLOCATOR, &this->device));
        KRAKEN_ASSERT_VALUE(this->device);

        for (uint32_t i{ 0 }; i < queueFamilyCount; i++)
        {
            uint32_t j{ 0 };
            if (i == this->graphicsQueueIndex)
            {
                vkGetDeviceQueue(this->device, this->graphicsQueueIndex, j, &this->graphicsQueue);
                j++;
            }
            if (i == this->computeQueueIndex)
            {
                vkGetDeviceQueue(this->device, this->computeQueueIndex, j, &this->computeQueue);
                j++;
            }
            if (i == this->presentQueueIndex)
            {
                vkGetDeviceQueue(this->device, this->presentQueueIndex, j, &this->presentQueue);
                j++;
            }
        }

    }

    VkPhysicalDevice Device::getPhysicalDevice() const
    {
        return physicalDevice;
    }

    VkDevice Device::getDevice() const
    {
        return device;
    }

    VkQueue Device::getGraphicsQueue() const
    {
        return graphicsQueue;
    }

    VkQueue Device::getComputeQueue() const
    {
        return computeQueue;
    }

    VkQueue Device::getPresentQueue() const
    {
        return presentQueue;
    }

    uint32_t Device::getGraphicsQueueIndex() const
    {
        return graphicsQueueIndex;
    }

    uint32_t Device::getComputeQueueIndex() const
    {
        return computeQueueIndex;
    }

    uint32_t Device::getPresentQueueIndex() const
    {
        return presentQueueIndex;
    }
}
