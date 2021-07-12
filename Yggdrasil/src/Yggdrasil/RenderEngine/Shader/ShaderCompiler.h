// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#include <vector>

namespace Ygg
{
    class CGraphicsDevice;
}

namespace Ygg::ShaderCompiler
{
    enum class ShaderType
    {
        VERT,
        TESC,
        TESE,
        GEOM,
        FRAG,
        COMP,
        TASKNV,
        MESHNV,
        RGEN,
        RINT,
        RAHIT,
        RCHIT,
        RMISS,
        RCALL
    };

    void Init();
    void Free();

    bool CompileShaderFromFile(const char* filename, std::vector<uint32_t>& spirvResult);
    bool CompileShaderFromString(const char* code, ShaderType type, std::vector<uint32_t>& spirvResult, const char* includePath = nullptr);

    SProgram CompileAndReflectShadersFromFiles(const std::vector<const char*>& files, CDescriptorSetLayoutCache& descriptorSetLayoutCache, const CGraphicsDevice& device);
}
