#pragma once
#include <spirv_cross/spirv_reflect.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Ygg::ShaderReflect
{
    spirv_cross::Compiler GetReflection(const std::vector<uint32_t>& spirv);

    // TODO: reflection system
}
