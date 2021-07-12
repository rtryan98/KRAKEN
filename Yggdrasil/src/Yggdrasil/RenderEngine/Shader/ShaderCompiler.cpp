// Copyright 2021 Robert Ryan. See LICENCE.md.

#include "Yggdrasil/pch.h"
#include "Yggdrasil/RenderEngine/Shader/ShaderReflect.h"
#include "Yggdrasil/RenderEngine/Shader/ShaderCompiler.h"
#include "Yggdrasil/RenderEngine/Shader/DirStackFileIncluder.h"
#include "Yggdrasil/RenderEngine/Shader/Shader.h"
#include "Yggdrasil/RenderEngine/GraphicsDevice.h"
#include "Yggdrasil/RenderEngine/Descriptor/DescriptorSetLayoutCache.h"
#include "Yggdrasil/Common/Util/FileUtil.h"

#include <filesystem>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

namespace Ygg::ShaderCompiler
{
    constexpr int32_t                           CLIENT_INPUT_SEMANTICS_VERSION { 120                          };
    constexpr glslang::EShTargetClientVersion   VULKAN_CLIENT_VERSION          { glslang::EShTargetVulkan_1_2 };
    constexpr glslang::EShTargetLanguageVersion TARGET_SPIRV                   { glslang::EShTargetSpv_1_5    };
    constexpr int32_t                           DEFAULT_VERSION                { 100                          };
    constexpr TBuiltInResource DEFAULT_T_BUILT_IN_RESOURCE
    {
        /* .MaxLights = */ 32,
        /* .MaxClipPlanes = */ 6,
        /* .MaxTextureUnits = */ 32,
        /* .MaxTextureCoords = */ 32,
        /* .MaxVertexAttribs = */ 64,
        /* .MaxVertexUniformComponents = */ 4096,
        /* .MaxVaryingFloats = */ 64,
        /* .MaxVertexTextureImageUnits = */ 32,
        /* .MaxCombinedTextureImageUnits = */ 80,
        /* .MaxTextureImageUnits = */ 32,
        /* .MaxFragmentUniformComponents = */ 4096,
        /* .MaxDrawBuffers = */ 32,
        /* .MaxVertexUniformVectors = */ 128,
        /* .MaxVaryingVectors = */ 8,
        /* .MaxFragmentUniformVectors = */ 16,
        /* .MaxVertexOutputVectors = */ 16,
        /* .MaxFragmentInputVectors = */ 15,
        /* .MinProgramTexelOffset = */ -8,
        /* .MaxProgramTexelOffset = */ 7,
        /* .MaxClipDistances = */ 8,
        /* .MaxComputeWorkGroupCountX = */ 65535,
        /* .MaxComputeWorkGroupCountY = */ 65535,
        /* .MaxComputeWorkGroupCountZ = */ 65535,
        /* .MaxComputeWorkGroupSizeX = */ 1024,
        /* .MaxComputeWorkGroupSizeY = */ 1024,
        /* .MaxComputeWorkGroupSizeZ = */ 64,
        /* .MaxComputeUniformComponents = */ 1024,
        /* .MaxComputeTextureImageUnits = */ 16,
        /* .MaxComputeImageUniforms = */ 8,
        /* .MaxComputeAtomicCounters = */ 8,
        /* .MaxComputeAtomicCounterBuffers = */ 1,
        /* .MaxVaryingComponents = */ 60,
        /* .MaxVertexOutputComponents = */ 64,
        /* .MaxGeometryInputComponents = */ 64,
        /* .MaxGeometryOutputComponents = */ 128,
        /* .MaxFragmentInputComponents = */ 128,
        /* .MaxImageUnits = */ 8,
        /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
        /* .MaxCombinedShaderOutputResources = */ 8,
        /* .MaxImageSamples = */ 0,
        /* .MaxVertexImageUniforms = */ 0,
        /* .MaxTessControlImageUniforms = */ 0,
        /* .MaxTessEvaluationImageUniforms = */ 0,
        /* .MaxGeometryImageUniforms = */ 0,
        /* .MaxFragmentImageUniforms = */ 8,
        /* .MaxCombinedImageUniforms = */ 8,
        /* .MaxGeometryTextureImageUnits = */ 16,
        /* .MaxGeometryOutputVertices = */ 256,
        /* .MaxGeometryTotalOutputComponents = */ 1024,
        /* .MaxGeometryUniformComponents = */ 1024,
        /* .MaxGeometryVaryingComponents = */ 64,
        /* .MaxTessControlInputComponents = */ 128,
        /* .MaxTessControlOutputComponents = */ 128,
        /* .MaxTessControlTextureImageUnits = */ 16,
        /* .MaxTessControlUniformComponents = */ 1024,
        /* .MaxTessControlTotalOutputComponents = */ 4096,
        /* .MaxTessEvaluationInputComponents = */ 128,
        /* .MaxTessEvaluationOutputComponents = */ 128,
        /* .MaxTessEvaluationTextureImageUnits = */ 16,
        /* .MaxTessEvaluationUniformComponents = */ 1024,
        /* .MaxTessPatchComponents = */ 120,
        /* .MaxPatchVertices = */ 32,
        /* .MaxTessGenLevel = */ 64,
        /* .MaxViewports = */ 16,
        /* .MaxVertexAtomicCounters = */ 0,
        /* .MaxTessControlAtomicCounters = */ 0,
        /* .MaxTessEvaluationAtomicCounters = */ 0,
        /* .MaxGeometryAtomicCounters = */ 0,
        /* .MaxFragmentAtomicCounters = */ 8,
        /* .MaxCombinedAtomicCounters = */ 8,
        /* .MaxAtomicCounterBindings = */ 1,
        /* .MaxVertexAtomicCounterBuffers = */ 0,
        /* .MaxTessControlAtomicCounterBuffers = */ 0,
        /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
        /* .MaxGeometryAtomicCounterBuffers = */ 0,
        /* .MaxFragmentAtomicCounterBuffers = */ 1,
        /* .MaxCombinedAtomicCounterBuffers = */ 1,
        /* .MaxAtomicCounterBufferSize = */ 16384,
        /* .MaxTransformFeedbackBuffers = */ 4,
        /* .MaxTransformFeedbackInterleavedComponents = */ 64,
        /* .MaxCullDistances = */ 8,
        /* .MaxCombinedClipAndCullDistances = */ 8,
        /* .MaxSamples = */ 4,
        /* .maxMeshOutputVerticesNV = */ 256,
        /* .maxMeshOutputPrimitivesNV = */ 512,
        /* .maxMeshWorkGroupSizeX_NV = */ 32,
        /* .maxMeshWorkGroupSizeY_NV = */ 1,
        /* .maxMeshWorkGroupSizeZ_NV = */ 1,
        /* .maxTaskWorkGroupSizeX_NV = */ 32,
        /* .maxTaskWorkGroupSizeY_NV = */ 1,
        /* .maxTaskWorkGroupSizeZ_NV = */ 1,
        /* .maxMeshViewCountNV = */ 4,
        /* .maxDualSourceDrawBuffersEXT = */ 1,

        /* .limits = */ {
        /* .nonInductiveForLoops = */ 1,
        /* .whileLoops = */ 1,
        /* .doWhileLoops = */ 1,
        /* .generalUniformIndexing = */ 1,
        /* .generalAttributeMatrixVectorIndexing = */ 1,
        /* .generalVaryingIndexing = */ 1,
        /* .generalSamplerIndexing = */ 1,
        /* .generalVariableIndexing = */ 1,
        /* .generalConstantMatrixVectorIndexing = */ 1,
        }
    };

    EShLanguage FindLanguage(const std::string& extension)
    {
        if (extension == "vert")
            return EShLangVertex;
        else if (extension == "tesc")
            return EShLangTessControl;
        else if (extension == "tese")
            return EShLangTessEvaluation;
        else if (extension == "geom")
            return EShLangGeometry;
        else if (extension == "frag")
            return EShLangFragment;
        else if (extension == "comp")
            return EShLangCompute;
        else if (extension == "rgen")
            return EShLangRayGen;
        else if (extension == "rahit")
            return EShLangAnyHit;
        else if (extension == "rchit")
            return EShLangClosestHit;
        else if (extension == "rmiss")
            return EShLangMiss;
        else if (extension == "rcall")
            return EShLangCallable;
        else if (extension == "mesh")
            return EShLangMeshNV;
        else if (extension == "task")
            return EShLangTaskNV;

        YGG_WARN("'{0}' is no valid extension name.", extension);
        return EShLangVertex;
    }

    const char* GetBinaryName(EShLanguage lang)
    {
        switch (lang)
        {
        case EShLangVertex:         return "vert.spv";
        case EShLangTessControl:    return "tesc.spv";
        case EShLangTessEvaluation: return "tese.spv";
        case EShLangGeometry:       return "geom.spv";
        case EShLangFragment:       return "frag.spv";
        case EShLangCompute:        return "comp.spv";
        case EShLangRayGen:         return "rgen.spv";
        case EShLangIntersect:      return "rint.spv";
        case EShLangAnyHit:         return "rahit.spv";
        case EShLangClosestHit:     return "rchit.spv";
        case EShLangMiss:           return "rmiss.spv";
        case EShLangCallable:       return "rcall.spv";
        case EShLangMeshNV:         return "mesh.spv";
        case EShLangTaskNV:         return "task.spv";
        default:
            YGG_WARN("'{0}' is not a valid language.", lang);
            return "";
        }
    }

    void Preprocess(glslang::TShader& shader, EShMessages messages, std::string& preprocessTarget, glslang::TShader::Includer& includer, const char* name = "Internal Shader")
    {
        if (!shader.preprocess(
            &DEFAULT_T_BUILT_IN_RESOURCE,
            DEFAULT_VERSION,
            ENoProfile,
            false,
            false,
            messages, &preprocessTarget,
            includer))
        {
            YGG_WARN("Failed to preprocess shader '{0}'.\nLog: '{1}'\nDebug Log: '{2}'",
                name, shader.getInfoLog(), shader.getInfoDebugLog());
        }
    }

    void Parse(glslang::TShader& shader, EShMessages messages, std::string& preprocessedGlsl, const char* name = "Internal Shader")
    {
        const char* preprocessedGlslCstr{ preprocessedGlsl.c_str() };
        shader.setStrings(&preprocessedGlslCstr, 1);

        if (!shader.parse(&DEFAULT_T_BUILT_IN_RESOURCE, DEFAULT_VERSION, false, messages))
        {
            YGG_WARN("Failed to parse shader '{0}'.\nLog: '{1}'\nDebug Log: '{2}'",
                name, shader.getInfoLog(), shader.getInfoDebugLog());
        }
    }

    void Link(glslang::TProgram& program, glslang::TShader& shader, EShMessages messages, const char* name = "Internal Shader")
    {
        program.addShader(&shader);
        if (!program.link(messages))
        {
            YGG_WARN("Failed to link shader '{0}'.\nLog: '{1}'\nDebug Log: '{2}'",
                name, shader.getInfoLog(), shader.getInfoDebugLog());
        }
    }

    void Init()
    {
        glslang::InitializeProcess();
    }

    void Free()
    {
        glslang::FinalizeProcess();
    }

    ShaderType ConvertEShLangToShaderType(EShLanguage lang)
    {
        switch (lang)
        {
        case EShLangVertex:         return ShaderType::VERT;
        case EShLangTessControl:    return ShaderType::TESC;
        case EShLangTessEvaluation: return ShaderType::TESE;
        case EShLangGeometry:       return ShaderType::GEOM;
        case EShLangFragment:       return ShaderType::FRAG;
        case EShLangCompute:        return ShaderType::COMP;
        case EShLangRayGen:         return ShaderType::RGEN;
        case EShLangIntersect:      return ShaderType::RINT;
        case EShLangAnyHit:         return ShaderType::RAHIT;
        case EShLangClosestHit:     return ShaderType::RCHIT;
        case EShLangMiss:           return ShaderType::RMISS;
        case EShLangCallable:       return ShaderType::RCALL;
        case EShLangMeshNV:         return ShaderType::MESHNV;
        case EShLangTaskNV:         return ShaderType::TASKNV;
        default:
            YGG_WARN("Unknown shader type! Defaulting to ShaderType::VERT.");
            return ShaderType::VERT;
            break;
        }
    }

    EShLanguage ConvertShaderTypeToEShLanguage(ShaderType type)
    {
        switch (type)
        {
        case Ygg::ShaderCompiler::ShaderType::VERT:   return EShLangVertex;
        case Ygg::ShaderCompiler::ShaderType::TESC:   return EShLangTessControl;
        case Ygg::ShaderCompiler::ShaderType::TESE:   return EShLangTessEvaluation;
        case Ygg::ShaderCompiler::ShaderType::GEOM:   return EShLangGeometry;
        case Ygg::ShaderCompiler::ShaderType::FRAG:   return EShLangFragment;
        case Ygg::ShaderCompiler::ShaderType::COMP:   return EShLangCompute;
        case Ygg::ShaderCompiler::ShaderType::TASKNV: return EShLangTaskNV;
        case Ygg::ShaderCompiler::ShaderType::MESHNV: return EShLangMeshNV;
        case Ygg::ShaderCompiler::ShaderType::RGEN:   return EShLangRayGen;
        case Ygg::ShaderCompiler::ShaderType::RINT:   return EShLangIntersect;
        case Ygg::ShaderCompiler::ShaderType::RAHIT:  return EShLangAnyHit;
        case Ygg::ShaderCompiler::ShaderType::RCHIT:  return EShLangClosestHit;
        case Ygg::ShaderCompiler::ShaderType::RMISS:  return EShLangMiss;
        case Ygg::ShaderCompiler::ShaderType::RCALL:  return EShLangCallable;
        default:
            YGG_WARN("Unknown shader type! Defaulting to EShLangVertex.");
            return EShLangVertex;
        }
    }

    bool CompileShaderFromFile(const char* filename, std::vector<uint32_t>& spirvResult)
    {
        std::string code{ FileUtil::ReadFileAsString(filename) };
        EShLanguage lang{ FindLanguage(FileUtil::FileExtension(filename)) };
        ShaderType type{ ConvertEShLangToShaderType(lang) };

        std::string directory{ std::filesystem::path(filename).parent_path().string() };

        YGG_TRACE("Compiling shader file '{0}'.", filename);
        return CompileShaderFromString(code.c_str(), type, spirvResult, directory.c_str(), filename);
    }

    bool CompileShaderFromString(const char* code, ShaderType type, std::vector<uint32_t>& spirvResult, const char* includePath, const char* name)
    {
        EShLanguage language{ ConvertShaderTypeToEShLanguage(type) };

        glslang::TShader shader{ language };
        shader.setStrings(&code, 1);
        shader.setEnvInput(glslang::EShSourceGlsl, language, glslang::EShClientVulkan, CLIENT_INPUT_SEMANTICS_VERSION);
        shader.setEnvClient(glslang::EShClientVulkan, VULKAN_CLIENT_VERSION);
        shader.setEnvTarget(glslang::EShTargetSpv, TARGET_SPIRV);

        EShMessages messages{ (EShMsgSpvRules | EShMsgVulkanRules) };
        Vendor::Glslang::DirStackFileIncluder includer{};

        if (includePath != nullptr)
        {
            includer.pushExternalLocalDirectory(includePath);
        }

        std::string preprocessedGlsl{};
        glslang::TProgram program{};

        Preprocess(shader, messages, preprocessedGlsl, includer, (name == nullptr) ? "Unnamed Shader" : name);
        Parse(shader, messages, preprocessedGlsl, (name == nullptr) ? "Unnamed Shader" : name);
        Link(program, shader, messages, (name == nullptr) ? "Unnamed Shader" : name);

        spv::SpvBuildLogger logger{};
        glslang::SpvOptions options{};
        options.disableOptimizer = false;

        glslang::GlslangToSpv(*program.getIntermediate(language), spirvResult, &logger, &options);

        return false;
    }

    SProgram CompileAndReflectShadersFromFiles(const std::vector<const char*>& files, CDescriptorSetLayoutCache& descriptorSetLayoutCache, const CGraphicsDevice& device)
    {
        SProgramReflectionWrapper wrapper{};
        wrapper.shaders.resize(files.size());
        for (uint32_t i{ 0 }; i < files.size(); i++)
        {
            if (CompileShaderFromFile(files[i], wrapper.shaders[i].spirv) != 0)
                YGG_ERROR("Failed to compile shader '{0}'!", files[i]);

        }
        ShaderReflect::ParseProgram(descriptorSetLayoutCache, wrapper, device);

        SProgram result{};
        result.shaders.resize(wrapper.shaders.size());
        for (uint32_t i{ 0 }; i < wrapper.shaders.size(); i++)
        {
            result.shaders[i] = wrapper.shaders[i].shader;

            VkShaderModuleCreateInfo createInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
            createInfo.codeSize = static_cast<uint32_t>(wrapper.shaders[i].spirv.size()) * sizeof(uint32_t);
            createInfo.pCode = wrapper.shaders[i].spirv.data();
            result.shaders[i].module = device.CreateShaderModule(&createInfo);
        }
        result.descriptorUpdateTemplate = wrapper.program.descriptorUpdateTemplate;
        result.pipelineBindPoint = wrapper.program.pipelineBindPoint;
        result.pipelineLayout = wrapper.program.pipelineLayout;
        result.pushConstantFlags = wrapper.program.pushConstantFlags;
        result.setLayouts = wrapper.program.setLayouts;

        return result;
    }
}
