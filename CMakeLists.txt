cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

set(ENGINE_PROJECT_NAME "Yggdrasil")
project(${ENGINE_PROJECT_NAME})

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/scripts")
include(YggdrasilUtils)

# Add dependencies
# Header only dependencies are added via target_include_dirs
# All dependencies are located in /vendor unless they are
# Models or other resource data

# Search for Vulkan
message(STATUS "Searching for Vulkan")
find_package(Vulkan REQUIRED)

if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found.")
endif()

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory("vendor/glfw")
add_subdirectory("vendor/spdlog")
add_subdirectory("vendor/glslang")
add_subdirectory("ThirdParty")
# add_subdirectory("vendor/SPIRV-Tools")
add_library(spirv-reflect vendor/spirv-reflect/spirv_reflect.c vendor/spirv-reflect/spirv_reflect.h)

# Clean project up a little
set_target_properties(uninstall PROPERTIES FOLDER vendor/other)
set_target_properties(HLSL GenericCodeGen glslang-default-resource-limits MachineIndependent
    OGLCompiler OSDependent SPIRV SPVRemapper spirv-remap glslangValidator glslang 
    glslang-build-info PROPERTIES FOLDER vendor/glslang)
set_target_properties(glfw PROPERTIES FOLDER vendor/glfw)
set_target_properties(spdlog PROPERTIES FOLDER vendor/spdlog)
set_target_properties(spirv-reflect PROPERTIES FOLDER vendor/SPIRV-Reflect)

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

yggdrasil_set_dirs(${ENGINE_NAME})

# Include Libraries to Yggdrasil
target_link_libraries(${ENGINE_NAME} PUBLIC glslang
                                            glfw
                                            Vulkan::Vulkan
                                            spdlog
                                            spirv-reflect
                                            SPIRV
                                            Yggdrasil-ThirdParty)
target_include_directories(${ENGINE_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Yggdrasil/src/" "vendor/glm/"
                                                                                      "vendor/"
                                                                                      "vendor/imgui/")

# Compile Options
if(MSVC)
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

yggdrasil_set_dirs(${ENGINE_SANDBOX_NAME})

if(MSVC)
    set_target_properties(${ENGINE_SANDBOX_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Sandbox")
endif()

# Include Libraries to YGGDRASIL-Sandbox
target_link_libraries(${ENGINE_SANDBOX_NAME} PRIVATE ${ENGINE_NAME})
target_include_directories(${ENGINE_SANDBOX_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/Sandbox/src")

# Compile Options
if(MSVC)
    target_compile_options(${ENGINE_SANDBOX_NAME} PRIVATE "/W4" "/MP" "/std:c++17" "/WX")
    # target_link_options(${ENGINE_SANDBOX_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${ENGINE_SANDBOX_NAME})
endif()
