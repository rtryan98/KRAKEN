#include "KRAKEN/core/graphics/vulkan/VulkanDevice.h"
#include <vector>
#include "KRAKEN/Defines.h"
#include "KRAKEN/core/graphics/vulkan/VulkanGlobals.h"

namespace kraken::vulkan
{
    void Device::free()
    {
        vkDestroyDevice(this->device, VK_CPU_ALLOCATOR);
    }

    void Device::init(VkInstance instance, VkSurfaceKHR surface)
    {
        selectPhysicalDevice(instance);
        logPhysicalDeviceInfo();
        findQueueFamilies(surface);
        createDevice();
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

    void Device::findQueueFamilies(VkSurfaceKHR surface)
    {
        uint32_t queueFamilyCount{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, queueFamilies.data());
        for (uint32_t i{ 0 }; i < queueFamilyCount; i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                this->graphicsQueueIndex = i;
            }
            if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                this->computeQueueIndex = i;
            }
            VkBool32 presentationSupport{ 0 };
            vkGetPhysicalDeviceSurfaceSupportKHR(this->physicalDevice, i, surface, &presentationSupport);
            if (presentationSupport)
            {
                this->presentQueueIndex = i;
            }
        }
    }

    void Device::createDevice()
    {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        graphicsQueueCreateInfo.queueFamilyIndex = this->graphicsQueueIndex;

        VkDeviceQueueCreateInfo computeQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        computeQueueCreateInfo.queueFamilyIndex = this->computeQueueIndex;

        VkDeviceQueueCreateInfo presentationQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        presentationQueueCreateInfo.queueFamilyIndex = this->presentQueueIndex;

        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

        vkCreateDevice(this->physicalDevice, &deviceCreateInfo, VK_CPU_ALLOCATOR, &this->device);
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
