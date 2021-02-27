cmake_minimum_required(VERSION 3.13...3.18 FATAL_ERROR)

set(ENGINE_PROJECT_NAME "Yggdrasil")
project(${ENGINE_PROJECT_NAME})

# Add dependencies
# Header only dependencies are added via target_include_dirs
# All dependencies are located in /vendor unless they are
# Models or other resource data
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory("vendor/glfw")
add_subdirectory("vendor/spdlog")
add_subdirectory("vendor/glslang")

# Clean project up a little
set_target_properties(uninstall PROPERTIES FOLDER vendor/other)
set_target_properties(HLSL GenericCodeGen glslang-default-resource-limits MachineIndependent
    OGLCompiler OSDependent SPIRV SPVRemapper spirv-remap glslangValidator glslang 
    glslang-build-info PROPERTIES FOLDER vendor/glslang)
set_target_properties(glfw PROPERTIES FOLDER vendor/glfw)
set_target_properties(spdlog PROPERTIES FOLDER vendor/spdlog)

set(OUTPUT_DIR_DEBUG "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/debug")
set(OUTPUT_DIR_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/debug")
set(OUTPUT_DIR_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/debug")
set(OUTPUT_DIR_RELEASE "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/debug")

# Yggdrasil library
file(GLOB_RECURSE "ENGINE_SOURCE" "${CMAKE_SOURCE_DIR}/Yggdrasil/src/*.cpp")
file(GLOB_RECURSE "ENGINE_HEADER" "${CMAKE_SOURCE_DIR}/Yggdrasil/src/*.h")

foreach(item IN ITEMS ${ENGINE_SOURCE} ${ENGINE_HEADER})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "/" "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

# Add Library
set(ENGINE_NAME "Yggdrasil")
add_library(${ENGINE_NAME} ${ENGINE_SOURCE} ${ENGINE_HEADER})

# Add Precompiled Header
if (MSVC)
    set_target_properties(${ENGINE_NAME} PROPERTIES COMPILE_FLAGS "/YuYggdrasil/pch.h")
    set_source_files_properties(${CMAKE_SOURCE_DIR}/Yggdrasil/src/Yggdrasil/pch.cpp PROPERTIES COMPILE_FLAGS "/YcYggdrasil/pch.h")
endif (MSVC)

# Output Directories
set_target_properties(${ENGINE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/debug/")
set_target_properties(${ENGINE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/relwithdebinfo/")
set_target_properties(${ENGINE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/minsizerel/")
set_target_properties(${ENGINE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/release/")

# Search for Vulkan
message(STATUS "Searching for Vulkan")
find_package(Vulkan REQUIRED)

if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found.")
endif()

# Include Libraries to Yggdrasil
target_link_libraries(${ENGINE_NAME} PUBLIC glslang glfw Vulkan::Vulkan spdlog)
target_include_directories(${ENGINE_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Yggdrasil/src/" "vendor/glm/"
                                                                                      "vendor/"
                                                                                      "vendor/imgui/")

# Compile Options
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(${ENGINE_NAME} PRIVATE "/W4" "/MP" "/std:c++17" "/WX" "/wd26812")
endif()

# Yggdrasil Sandbox
file(GLOB_RECURSE "ENGINE_SANDBOX_SOURCE" "${CMAKE_SOURCE_DIR}/Sandbox/src/*.cpp" "${CMAKE_SOURCE_DIR}/Sandbox/src/*.h")
foreach(item IN ITEMS ${ENGINE_SANDBOX_SOURCE})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "/" "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

set(ENGINE_SANDBOX_NAME "Yggdrasil-Sandbox")
add_executable(${ENGINE_SANDBOX_NAME} ${ENGINE_SANDBOX_SOURCE})

# Output Directories
set_target_properties(${ENGINE_SANDBOX_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/debug/")
set_target_properties(${ENGINE_SANDBOX_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/relwithdebinfo/")
set_target_properties(${ENGINE_SANDBOX_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/minsizerel/")
set_target_properties(${ENGINE_SANDBOX_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/release/")

if(MSVC)
    set_target_properties(${ENGINE_SANDBOX_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Sandbox")
endif()

# Include Libraries to KRAKEN-Sandbox
target_link_libraries(${ENGINE_SANDBOX_NAME} PRIVATE ${ENGINE_NAME})
target_include_directories(${ENGINE_SANDBOX_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/Sandbox/src")

# Compile Options
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(${ENGINE_SANDBOX_NAME} PRIVATE "/W4" "/MP" "/std:c++17" "/WX")
    # target_link_options(${ENGINE_SANDBOX_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
endif()
