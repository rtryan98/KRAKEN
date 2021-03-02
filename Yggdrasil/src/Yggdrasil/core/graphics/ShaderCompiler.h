#pragma once
#include <vector>
#include <string>

namespace yggdrasil::shadercompiler
{
    void init();
    void free();
    std::vector<uint32_t> compileGLSL(const std::string& fileName);
}
