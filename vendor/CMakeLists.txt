# Copyright 2021 Robert Ryan. See LICENCE.md.

add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/spdlog")
set_target_properties(spdlog
    PROPERTIES FOLDER vendor/spdlog)

set(ENABLE_CTEST OFF)
add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/glslang")
set_target_properties(HLSL GenericCodeGen glslang-default-resource-limits MachineIndependent
    OGLCompiler OSDependent SPIRV SPVRemapper spirv-remap glslangValidator glslang 
    PROPERTIES FOLDER vendor/glslang)
set(ENABLE_CTEST ON)
