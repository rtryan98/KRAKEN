#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/memory/Allocator.h"
#include "Yggdrasil/core/graphics/Device.h"
#include "Yggdrasil/core/util/Log.h"

namespace yggdrasil::graphics::memory
{
    uint32_t getMemoryTypeIndex(const Device& device, VkMemoryPropertyFlags flags)
    {
        auto& memoryProperties{ device.memory.properties };
        for (uint32_t i{ 0 }; i < memoryProperties.memoryTypeCount; i++)
        {
            auto& memoryType{ memoryProperties.memoryTypes[i] };
            if ((memoryType.propertyFlags & flags) == flags)
            {
                return i;
            }
        }
        YGGDRASIL_CORE_WARN("No valid memory type found.");
        return ~0u;
    }
}
