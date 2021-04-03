#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/Device.h"
#include "Yggdrasil/core/graphics/Util.h"
#include "Yggdrasil/core/graphics/memory/Descriptor.h"

#include <algorithm>

namespace ygg::graphics::memory
{
    void DescriptorAllocator::createPoolSizes()
    {
        this->poolSizes.push_back({ VK_DESCRIPTOR_TYPE_SAMPLER,                 512 });
        this->poolSizes.push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4096 });
        this->poolSizes.push_back({ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          4096 });
        this->poolSizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1024 });
        this->poolSizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1024 });
        this->poolSizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1024 });
        this->poolSizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         2048 });
        this->poolSizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         2048 });
        this->poolSizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1024 });
        this->poolSizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1024 });
        this->poolSizes.push_back({ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,        512 });
        // TODO: add RTX / Turing pool sizes if available.
    }

    void DescriptorAllocator::create(const Device* const pDevice)
    {
        this->device = pDevice;
        createPoolSizes();
    }

    VkDescriptorPool DescriptorAllocator::createPool(uint32_t count, VkDescriptorPoolCreateFlags flags)
    {
        VkDescriptorPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        createInfo.poolSizeCount = static_cast<uint32_t>(this->poolSizes.size());
        createInfo.pPoolSizes = this->poolSizes.data();
        createInfo.maxSets = count;
        createInfo.flags = flags;
        VkDescriptorPool result{};
        VK_CHECK( vkCreateDescriptorPool(this->device->logical, &createInfo, VK_CPU_ALLOCATOR, &result) );
        return result;
    }

    void DescriptorAllocator::destroy()
    {
        for (auto pool : this->freePools)
        {
            util::destroy(&pool, vkDestroyDescriptorPool, this->device->logical);
        }
        for (auto pool : this->usedPools)
        {
            util::destroy(&pool, vkDestroyDescriptorPool, this->device->logical);
        }
    }

    void DescriptorAllocator::reset()
    {
        for (auto pool : this->usedPools)
        {
            vkResetDescriptorPool(this->device->logical, pool, 0);
        }
        this->freePools = this->usedPools;
        this->usedPools.clear();
        this->currentPool = VK_NULL_HANDLE;
    }

    bool_t DescriptorAllocator::allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout)
    {
        if (this->currentPool == VK_NULL_HANDLE)
        {
            this->currentPool = grabPool();
            this->usedPools.push_back(this->currentPool);
        }

        VkDescriptorSetAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        allocateInfo.pSetLayouts = &layout;
        allocateInfo.descriptorPool = this->currentPool;
        allocateInfo.descriptorSetCount = 1;

        VkResult allocateResult{ vkAllocateDescriptorSets(this->device->logical, &allocateInfo, set) };

        bool_t requiresReallocation{ false };
        switch (allocateResult)
        {
        case VK_SUCCESS:
            return true;
        case VK_ERROR_FRAGMENTED_POOL:
            [[fallthrough]];
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            requiresReallocation = true;
            break;
        default:
            VK_CHECK(allocateResult);
            return false;
        }

        if (requiresReallocation)
        {
            this->currentPool = grabPool();
            usedPools.push_back(this->currentPool);
            allocateResult = vkAllocateDescriptorSets(this->device->logical, &allocateInfo, set);
            if (allocateResult == VK_SUCCESS)
            {
                return true;
            }
        }
        VK_CHECK(allocateResult);
        return false;
    }

    VkDescriptorPool DescriptorAllocator::grabPool()
    {
        if (this->freePools.size() > 0)
        {
            VkDescriptorPool pool{ this->freePools.back() };
            this->freePools.pop_back();
            return pool;
        }
        else
        {
            return createPool(1024, 0);
        }
    }

    std::size_t DescriptorLayoutCache::DescriptorLayoutHash::operator()(const DescriptorLayoutInfo& k) const
    {
        return k.hash();
    }

    bool_t DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const
    {
        if (this->bindings.size() != other.bindings.size())
        {
            return false;
        }
        for (uint32_t i{ 0 }; i < this->bindings.size(); i++)
        {
            if (this->bindings[i].binding != other.bindings[i].binding                 ||
                this->bindings[i].descriptorType != other.bindings[i].descriptorType   ||
                this->bindings[i].descriptorCount != other.bindings[i].descriptorCount ||
                this->bindings[i].stageFlags != other.bindings[i].stageFlags)
            {
                return false;
            }
        }
        return true;
    }

    std::size_t DescriptorLayoutCache::DescriptorLayoutInfo::hash() const
    {
        std::size_t result{ std::hash<std::size_t>()(this->bindings.size()) };
        for (auto& binding : this->bindings)
        {
            result ^= std::hash<std::size_t>()(
                binding.binding               |
                binding.descriptorType << 8   |
                binding.descriptorCount << 12 |
                binding.stageFlags << 16
                );
        }
        return result;
    }

    void DescriptorLayoutCache::create(const Device* const pDevice)
    {
        this->device = pDevice;
    }

    VkDescriptorSetLayout DescriptorLayoutCache::createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* createInfo)
    {
        DescriptorLayoutInfo layoutInfo{};
        layoutInfo.bindings.reserve(createInfo->bindingCount);
        bool_t isSorted{ true };
        int32_t lastBinding{ -1 };

        for (uint32_t i{ 0 }; i < createInfo->bindingCount; i++)
        {
            if (static_cast<int32_t>(createInfo->pBindings[i].binding) > lastBinding)
            {
                lastBinding = createInfo->pBindings[i].binding;
            }
            else
            {
                isSorted = false;
            }
        }

        if (!isSorted)
        {
            std::sort(layoutInfo.bindings.begin(), layoutInfo.bindings.end(), [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b)
                {
                    return a.binding < b.binding;
                });
        }

        auto it{ this->cache.find(layoutInfo) };
        if (it != this->cache.end())
        {
            return (*it).second;
        }

        VkDescriptorSetLayout layout{};
        vkCreateDescriptorSetLayout(this->device->logical, createInfo, VK_CPU_ALLOCATOR, &layout);
        this->cache[layoutInfo] = layout;
        return layout;
    }

    DescriptorFactory DescriptorFactory::begin(DescriptorLayoutCache* pCache, DescriptorAllocator* pAllocator)
    {
        DescriptorFactory factory{};
        factory.cache = pCache;
        factory.allocator = pAllocator;
        return factory;
    }

    DescriptorFactory& DescriptorFactory::bindBuffer(uint32_t binding, VkDescriptorBufferInfo* info, VkDescriptorType type, VkShaderStageFlags shaderStageFlags)
    {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = type;
        layoutBinding.stageFlags = shaderStageFlags;
        layoutBinding.binding = binding;

        this->bindings.push_back(layoutBinding);

        VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pBufferInfo = info;
        write.dstBinding = binding;

        this->writes.push_back(write);
        return *this;
    }

    DescriptorFactory& DescriptorFactory::bindImage(uint32_t binding, VkDescriptorImageInfo* info, VkDescriptorType type, VkShaderStageFlags shaderStageFlags)
    {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = type;
        layoutBinding.stageFlags = shaderStageFlags;
        layoutBinding.binding = binding;

        this->bindings.push_back(layoutBinding);

        VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pImageInfo = info;
        write.dstBinding = binding;

        this->writes.push_back(write);
        return *this;
    }

    bool_t DescriptorFactory::build(VkDescriptorSet* set, VkDescriptorSetLayout* layout)
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        layoutInfo.pBindings = this->bindings.data();
        layoutInfo.bindingCount = static_cast<uint32_t>(this->bindings.size());
        *layout = this->cache->createDescriptorSetLayout(&layoutInfo);
        bool_t success{ this->allocator->allocate(set, *layout) };
        if (!success)
        {
            return false;
        }
        for (VkWriteDescriptorSet& write : this->writes)
        {
            write.dstSet = *set;
        }
        vkUpdateDescriptorSets(this->cache->device->logical, static_cast<uint32_t>(this->writes.size()), this->writes.data(), 0, nullptr);
        return true;
    }

    bool_t DescriptorFactory::build(VkDescriptorSet* set)
    {
        VkDescriptorSetLayout layout{};
        return build(set, &layout);
    }
}
