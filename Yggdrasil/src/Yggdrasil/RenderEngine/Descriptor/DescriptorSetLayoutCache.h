// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#include <unordered_map>
#include <vulkan/vulkan.h>

/// The code of the DescriptorSetLayoutCache is adapted from VkGuide.
namespace Ygg
{
    struct SDescriptorSetLayoutInfo
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings{};

        /// @brief Compares this SDescriptorSetLayoutInfo with another one.
        /// The bindings field must be sorted before the comparison.
        /// @param other The other SDescriptorSetLayoutInfo with which to compare this object.
        /// @return true if the objects are the same.
        bool operator==(const SDescriptorSetLayoutInfo& other) const;
    };

    struct SDescriptorLayoutHash
    {
        std::size_t operator()(const SDescriptorSetLayoutInfo& k) const;
    };

    class CGraphicsDevice;

    class CDescriptorSetLayoutCache
    {
    public:
        VkDescriptorSetLayout CreateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* pCreateInfo);
        void Init(const CGraphicsDevice& device);
        void Destroy();
    private:
        std::unordered_map<SDescriptorSetLayoutInfo, VkDescriptorSetLayout, SDescriptorLayoutHash> m_layoutCache{};

        /// @brief non owned CGraphicsDevice
        const CGraphicsDevice* a_device;
    };
}
