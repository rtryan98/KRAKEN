#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/memory/Buffer.h"
#include "Yggdrasil/core/graphics/memory/Allocator.h"
#include "Yggdrasil/core/graphics/Util.h"
#include "Yggdrasil/core/graphics/Renderer.h"
#include "Yggdrasil/core/util/Log.h"

namespace yggdrasil::graphics::memory
{
    void Buffer::create(const Renderer* const renderer, uint32_t bufferType, uint32_t bufferUsage, uint64_t bufferSize)
    {

        const Device& device{ renderer->getContext().device };
        this->usage = bufferUsage;
        this->type = bufferType;

        VkBufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 1;
        createInfo.pQueueFamilyIndices = &device.queues.rasterizerQueueFamilyIndex;
        createInfo.size = bufferSize;

        if (this->type & BUFFER_TYPE_VERTEX)
        {
            createInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        if (this->type & BUFFER_TYPE_INDEX)
        {
            createInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        if (this->type & BUFFER_TYPE_INDIRECT)
        {
            createInfo.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }
        if (this->type & BUFFER_TYPE_STORAGE)
        {
            createInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }
        if (this->type & BUFFER_TYPE_UNIFORM)
        {
            createInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            // TODO: alignment?
            createInfo.size += createInfo.size & device.properties.limits.minUniformBufferOffsetAlignment;
            createInfo.size *= static_cast<uint32_t>(renderer->getContext().screen.swapchainImages.size());
        }

        VK_CHECK( vkCreateBuffer(device.logical, &createInfo, VK_CPU_ALLOCATOR, &this->handle) );

        // TODO: allocate
        VkMemoryRequirements memoryRequirements{};
        vkGetBufferMemoryRequirements(device.logical, this->handle, &memoryRequirements);
        // TODO: move to allocator
        VkMemoryAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocateInfo.allocationSize = memoryRequirements.size;

        if (this->type & BUFFER_TYPE_UNIFORM)
        {
            allocateInfo.memoryTypeIndex = getMemoryTypeIndex(device,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            if (allocateInfo.memoryTypeIndex == ~0u)
            {
                allocateInfo.memoryTypeIndex = getMemoryTypeIndex(device,
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            }
        }
        else
        {
            allocateInfo.memoryTypeIndex = getMemoryTypeIndex(device,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        }

        VK_CHECK(vkAllocateMemory(device.logical, &allocateInfo, VK_CPU_ALLOCATOR, &this->memory));

        bindMemory(device);
        if (this->type & BUFFER_TYPE_UNIFORM ||
            this->usage & BUFFER_USAGE_UPDATE_EVERY_FRAME)
        {
            map(device);
        }
    }

    void Buffer::bindMemory(const Device& device)
    {
        VK_CHECK( vkBindBufferMemory(device.logical, this->handle, this->memory, 0) );
    }

    void* Buffer::map(const Device& device)
    {
        if (this->data == nullptr)
        {
            VK_CHECK( vkMapMemory(device.logical, this->memory, 0, VK_WHOLE_SIZE, 0, &this->data) );
        }
        return this->data;
    }

    void Buffer::unmap(const Device& device)
    {
        if (this->data != nullptr)
        {
            vkUnmapMemory(device.logical, this->memory);
            this->data = nullptr;
        }
    }

    void Buffer::flush(const Device& device)
    {
        if (this->data == nullptr)
        {
            YGGDRASIL_CORE_WARN("Attempting to flush unmapped memory.");
            return;
        }

        VkMappedMemoryRange range{};
        range.offset = 0;
        range.size = VK_WHOLE_SIZE;
        range.memory = this->memory;

        VK_CHECK( vkFlushMappedMemoryRanges(device.logical, 1, &range) );
    }

    void Buffer::invalidate(const Device& device)
    {
        if (this->data == nullptr)
        {
            YGGDRASIL_CORE_WARN("Attempting to invalidate unmapped memory.");
            return;
        }

        VkMappedMemoryRange range{};
        range.offset = 0;
        range.size = VK_WHOLE_SIZE;
        range.memory = this->memory;

        VK_CHECK( vkInvalidateMappedMemoryRanges(device.logical, 1, &range) );
    }

    void Buffer::destroy(const Device& device)
    {
        util::destroy(&this->handle, vkDestroyBuffer, device.logical);
        if (this->memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device.logical, this->memory, VK_CPU_ALLOCATOR);
            this->memory = VK_NULL_HANDLE;
        }

    }
}
