#pragma once
#include <vector>
#include <string>

namespace yggdrasil::graphics::shadercompiler
{
    void init();
    void free();
    std::vector<uint32_t> compileGlsl(const std::string& fileName);
    std::vector<uint32_t> compileGlslInternal(const std::string& code, const char* stage);
}
