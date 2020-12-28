cmake_minimum_required(VERSION 3.13...3.18 FATAL_ERROR)

set(KRAKEN_NAME "KRAKEN")
project(${KRAKEN_NAME})

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

# KRAKEN library
file(GLOB_RECURSE "KRAKEN_LIB_SOURCE" "${CMAKE_SOURCE_DIR}/KRAKEN/src/*.cpp")
file(GLOB_RECURSE "KRAKEN_LIB_HEADER" "${CMAKE_SOURCE_DIR}/KRAKEN/include/*.h")

# Group files together. May look better in some IDEs
foreach(item IN ITEMS ${KRAKEN_LIB_SOURCE} ${KRAKEN_LIB_HEADER})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "/" "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

set(KRAKEN_LIB_NAME "KRAKEN")
add_library(${KRAKEN_LIB_NAME} ${KRAKEN_LIB_SOURCE} ${KRAKEN_LIB_HEADER})

# Output Directories
set_target_properties(${KRAKEN_LIB_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/${KRAKEN_LIB_NAME}/debug/")
set_target_properties(${KRAKEN_LIB_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/${KRAKEN_LIB_NAME}/relwithdebinfo/")
set_target_properties(${KRAKEN_LIB_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/${KRAKEN_LIB_NAME}/minsizerel/")
set_target_properties(${KRAKEN_LIB_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/${KRAKEN_LIB_NAME}/release/")

# Search for Vulkan
message(STATUS "Searching for Vulkan")
find_package(Vulkan REQUIRED)

if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found.")
endif()

# Include Libraries to KRAKEN
target_link_libraries(${KRAKEN_LIB_NAME} PUBLIC glslang spirv-tools glfw Vulkan::Vulkan)
target_include_directories(${KRAKEN_LIB_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/KRAKEN/include/" "vendor/glm/" )

# Compile Options
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(${KRAKEN_LIB_NAME} PRIVATE "/W4" "/MP" "/std:c++17" "/WX")
endif()

# KRAKEN Sandbox
file(GLOB_RECURSE "KRAKEN_SANDBOX_SOURCE" "${CMAKE_SOURCE_DIR}/Sandbox/src/*.cpp" "${CMAKE_SOURCE_DIR}/Sandbox/src/*.h")

# Group files together. May look better in some IDEs
foreach(item IN ITEMS ${KRAKEN_SANDBOX_SOURCE})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "/" "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

set(KRAKEN_SANDBOX_NAME "KRAKEN-Sandbox")
add_executable(${KRAKEN_SANDBOX_NAME} ${KRAKEN_SANDBOX_SOURCE})

# Output Directories
set_target_properties(${KRAKEN_SANDBOX_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/${KRAKEN_LIB_NAME}/debug/")
set_target_properties(${KRAKEN_SANDBOX_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/${KRAKEN_LIB_NAME}/relwithdebinfo/")
set_target_properties(${KRAKEN_SANDBOX_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/${KRAKEN_LIB_NAME}/minsizerel/")
set_target_properties(${KRAKEN_SANDBOX_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/${KRAKEN_LIB_NAME}/release/")

# Include Libraries to KRAKEN-Sandbox
target_link_libraries(${KRAKEN_SANDBOX_NAME} PRIVATE ${KRAKEN_LIB_NAME})
target_include_directories(${KRAKEN_SANDBOX_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/KRAKEN/include/" "vendor/glm/" )

# Compile Options
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(${KRAKEN_SANDBOX_NAME} PRIVATE "/W4" "/MP" "/std:c++17" "/WX")
    # target_link_options(${KRAKEN_SANDBOX_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
endif()
