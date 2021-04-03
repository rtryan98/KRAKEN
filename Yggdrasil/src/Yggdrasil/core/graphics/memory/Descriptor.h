#pragma once
#include "Yggdrasil/Types.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>

namespace ygg::graphics
{
    class Device;
}

namespace ygg::graphics::memory
{
    class DescriptorAllocator
    {
    public:
        void create(const Device* const pDevice);
        VkDescriptorPool createPool(uint32_t count, VkDescriptorPoolCreateFlags flags);
        void destroy();
        void reset();
        bool_t allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout);

    private:
        VkDescriptorPool grabPool();
        void createPoolSizes();

    private:
        VkDescriptorPool currentPool{ VK_NULL_HANDLE };

        /// @brief non-owned device.
        const Device* device{ nullptr };

        std::vector<VkDescriptorPoolSize> poolSizes{};
        std::vector<VkDescriptorPool> usedPools{};
        std::vector<VkDescriptorPool> freePools{};
    };

    class DescriptorLayoutCache
    {
    public:
        void create(const Device* const pDevice);
        VkDescriptorSetLayout createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* createInfo);

        struct DescriptorLayoutInfo
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings{};
            bool_t operator==(const DescriptorLayoutInfo& other) const;
            std::size_t hash() const;
        };
    private:

        struct DescriptorLayoutHash
        {
            std::size_t operator()(const DescriptorLayoutInfo& k) const;
        };

        std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> cache{};

        /// @brief non-owned device.
        const Device* device{ nullptr };

        friend class DescriptorFactory;
    };

    class DescriptorFactory
    {
    public:
        static DescriptorFactory begin(DescriptorLayoutCache* pCache, DescriptorAllocator* pAllocator);
        DescriptorFactory& bindBuffer(uint32_t binding, VkDescriptorBufferInfo* info, VkDescriptorType type, VkShaderStageFlags shaderStageFlags);
        DescriptorFactory& bindImage(uint32_t binding, VkDescriptorImageInfo* info, VkDescriptorType type, VkShaderStageFlags shaderStageFlags);
        bool_t build(VkDescriptorSet* set, VkDescriptorSetLayout* layout);
        bool_t build(VkDescriptorSet* set);
    private:
        DescriptorLayoutCache* cache{};
        DescriptorAllocator* allocator{};

        std::vector<VkWriteDescriptorSet> writes{};
        std::vector<VkDescriptorSetLayoutBinding> bindings{};
    };
}
