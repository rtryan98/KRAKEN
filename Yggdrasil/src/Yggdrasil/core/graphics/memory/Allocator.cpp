#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/memory/Allocator.h"
#include "Yggdrasil/core/graphics/Device.h"
#include "Yggdrasil/core/util/Log.h"

namespace ygg::graphics::memory
{
    uint32_t getMemoryTypeIndex(const Device& device, VkMemoryPropertyFlags required, VkMemoryPropertyFlags excluded)
    {
        auto& memoryProperties{ device.memory.properties };
        for (uint32_t i{ 0 }; i < memoryProperties.memoryTypeCount; i++)
        {
            auto& memoryType{ memoryProperties.memoryTypes[i] };
            if ((memoryType.propertyFlags & required) == required)
            {
                if (excluded != 0 &&
                    (memoryType.propertyFlags & excluded) == excluded)
                {
                    continue;
                }
                return i;
            }
        }
        YGGDRASIL_CORE_WARN("No valid memory type found.");
        return ~0u;
    }
}
