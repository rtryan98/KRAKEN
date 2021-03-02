#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/ShaderCompiler.h"
#include "Yggdrasil/core/util/Log.h"
#include "Yggdrasil/core/util/FileUtil.h"
#include "Yggdrasil/Types.h"

#include <glslang/Public/ShaderLang.h>
#include <glslang/StandAlone/DirStackFileIncluder.h>
#include <SPIRV/GlslangToSpv.h>

namespace yggdrasil::shadercompiler
{
    constexpr int32_t CLIENT_INPUT_SEMANTICS_VERSION                { 120                          }; // Vulkan 120
    constexpr glslang::EShTargetClientVersion VULKAN_CLIENT_VERSION { glslang::EShTargetVulkan_1_2 }; // Vulkan 1.2
    constexpr glslang::EShTargetLanguageVersion TARGET_VERSION      { glslang::EShTargetSpv_1_5    }; //  SPIRV 1.5
    constexpr int32_t DEFAULT_VERSION                               { 100                          };
    constexpr TBuiltInResource DEFAULT_T_BUILT_IN_RESOURCE = {
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

    EShLanguage findLanguage(const std::string& extension)
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

        YGGDRASIL_CORE_WARN("'{0}' is no valid extension name.", extension);
        return EShLangVertex;
    }

    const char* getBinaryName(EShLanguage lang)
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
            YGGDRASIL_CORE_WARN("'{0}' is not a valid language.", lang);
            return "";
        }
    }

    std::string extension(const std::string& fileName)
    {
        const uint32_t dot{ static_cast<uint32_t>(fileName.find('.')) };
        return (dot == std::string::npos)
            ? ""
            : fileName.substr(fileName.rfind('.') + 1);
    }

    bool_t isInitialized{ false };

    void init()
    {
        glslang::InitializeProcess();
        isInitialized = true;
    }

    void free()
    {
        glslang::FinalizeProcess();
        isInitialized = false;
    }

    void preprocess(glslang::TShader& shader, EShMessages messages, std::string& preprocessTarget, DirStackFileIncluder includer, const char* name)
    {
        if (!shader.preprocess(&DEFAULT_T_BUILT_IN_RESOURCE, DEFAULT_VERSION, ENoProfile, false, false, messages, &preprocessTarget, includer))
        {
            YGGDRASIL_CORE_WARN("Failed to preprocess shader '{0}'.\nLog: '{1}'\nDebug Log: '{2}'",
                name,
                shader.getInfoLog(),
                shader.getInfoDebugLog());
        }
    }

    void preprocess(glslang::TShader& shader, EShMessages messages, std::string& preprocessTarget, DirStackFileIncluder includer)
    {
        preprocess(shader, messages, preprocessTarget, includer, "Internal shader");
    }

    void parse(glslang::TShader& shader, EShMessages messages, std::string& preprocessedGlsl, const char* name)
    {
        const char* preprocessedGlslCstr{ preprocessedGlsl.c_str() };
        shader.setStrings(&preprocessedGlslCstr, 1);

        if (!shader.parse(&DEFAULT_T_BUILT_IN_RESOURCE, DEFAULT_VERSION, false, messages))
        {
            YGGDRASIL_CORE_WARN("Failed to parse shader '{0}'.\nLog: '{1}'\nDebug Log: '{2}'",
                name,
                shader.getInfoLog(),
                shader.getInfoDebugLog());
        }
    }

    void parse(glslang::TShader& shader, EShMessages messages, std::string& preprocessedGlsl)
    {
        parse(shader, messages, preprocessedGlsl, "Internal shader");
    }

    void link(glslang::TProgram& program, glslang::TShader& shader, EShMessages messages, const char* name)
    {
        program.addShader(&shader);

        if (!program.link(messages))
        {
            YGGDRASIL_CORE_WARN("Failed to link shader '{0}'.\nLog: '{1}'\nDebug Log: '{2}'",
                name,
                shader.getInfoLog(),
                shader.getInfoDebugLog());
        }
    }

    void link(glslang::TProgram& program, glslang::TShader& shader, EShMessages messages)
    {
        link(program, shader, messages, "Internal shader");
    }

    std::vector<uint32_t> compileGLSL(const std::string& fileName)
    {
        std::vector<uint32_t> spirv{};
        if (!isInitialized)
        {
            YGGDRASIL_CORE_ERROR("Tried to compile GLSL shader without initializing the shader compiler.\nUse yggdrasil::shadercompiler::init() to initialize the shader compiler.");
            return spirv;
        }
        std::string glsl{ fileutil::readStringFromFile(fileName) };
        const char* glslCstr{ glsl.c_str() };

        EShLanguage shaderStage{ findLanguage( extension(fileName) ) };

        glslang::TShader shader{ shaderStage };
        shader.setStrings(&glslCstr, 1);
        shader.setEnvInput(glslang::EShSourceGlsl, shaderStage, glslang::EShClientVulkan, CLIENT_INPUT_SEMANTICS_VERSION);
        shader.setEnvClient(glslang::EShClientVulkan, VULKAN_CLIENT_VERSION);
        shader.setEnvTarget(glslang::EShTargetSpv, TARGET_VERSION);

        EShMessages messages{ (EShMsgSpvRules | EShMsgVulkanRules) };
        DirStackFileIncluder includer{};
        // includer.pushExternalLocalDirectory(fileName);
        std::string preprocessedGlsl{};
        glslang::TProgram program{};

        preprocess(shader, messages, preprocessedGlsl, includer, fileName.c_str());
        parse(shader, messages, preprocessedGlsl, fileName.c_str());
        link(program, shader, messages, fileName.c_str());

        spv::SpvBuildLogger logger{};
        glslang::SpvOptions options{};
        glslang::GlslangToSpv(*program.getIntermediate( shaderStage ), spirv, &logger, &options);

        return spirv;
    }
}
