// Copyright 2021 Robert Ryan. See LICENCE.md.

#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/Descriptor/DescriptorSetLayoutCache.h"
#include "Yggdrasil/RenderEngine/GraphicsDevice.h"

namespace Ygg
{
    bool SDescriptorSetLayoutInfo::operator==(const SDescriptorSetLayoutInfo& other) const
    {
        if (this->bindings.size() != other.bindings.size())
        {
            return false;
        }
        for (uint32_t i{ 0 }; i < this->bindings.size(); i++)
        {
            const auto& thisCurrentElement{ this->bindings[i] };
            const auto& otherCurrentElement{ other.bindings[i] };
            if (thisCurrentElement.binding            != otherCurrentElement.binding ||
                thisCurrentElement.descriptorCount    != otherCurrentElement.descriptorCount ||
                thisCurrentElement.descriptorType     != otherCurrentElement.descriptorType ||
                thisCurrentElement.stageFlags         != otherCurrentElement.stageFlags)
            {
                return false;
            }
        }
        return true;
    }

    std::size_t SDescriptorLayoutHash::operator()(const SDescriptorSetLayoutInfo& k) const
    {
        std::size_t result{ std::hash<std::size_t>()(k.bindings.size()) };

        for (const auto& binding : k.bindings)
        {
            result ^= binding.binding |
                binding.descriptorCount << 8 |
                binding.descriptorType << 16 |
                binding.stageFlags << 24;
        }

        return result;
    }

    VkDescriptorSetLayout CDescriptorSetLayoutCache::CreateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* pCreateInfo)
    {
        SDescriptorSetLayoutInfo info{};
        info.bindings.reserve(pCreateInfo->bindingCount);

        bool sorted{ true };
        int32_t lastBinding{ -1 };

        for (uint32_t i{ 0 }; i < pCreateInfo->bindingCount; i++)
        {
            info.bindings.push_back(pCreateInfo->pBindings[i]);
            if (static_cast<int32_t>(pCreateInfo->pBindings[i].binding) > lastBinding)
            {
                lastBinding = pCreateInfo->pBindings[i].binding;
            }
            else
            {
                sorted = false;
            }
        }

        if (!sorted)
        {
            std::sort(info.bindings.begin(), info.bindings.end(),
                [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b) {
                    return a.binding < b.binding;
                });
        }

        auto it{ this->m_layoutCache.find(info) };
        if (it != this->m_layoutCache.end())
        {
            return (*it).second;
        }
        VkDescriptorSetLayout result{ this->a_device->CreateDescriptorSetLayout(pCreateInfo) };
        this->m_layoutCache[info] = result;
        return result;
    }

    void CDescriptorSetLayoutCache::Init(const CGraphicsDevice& device)
    {
        this->a_device = &device;
    }

    void CDescriptorSetLayoutCache::Destroy()
    {
        for (auto& layout : this->m_layoutCache)
        {
            this->a_device->DestroyDescriptorSetLayout(&layout.second);
        }
        this->m_layoutCache.clear();
    }
}
