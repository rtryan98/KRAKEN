#include "Yggdrasil/pch.h"
#include "ShaderReflect.h"

namespace Ygg::ShaderReflect
{
    spirv_cross::Compiler GetReflection(const std::vector<uint32_t>& spirv)
    {
        return spirv_cross::Compiler(spirv.data(), spirv.size() * sizeof(uint32_t));
    }
}
