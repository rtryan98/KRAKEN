cmake_minimum_required(VERSION 3.13...3.18 FATAL_ERROR)

set(KRAKEN_NAME "KRAKEN")
project(${KRAKEN_NAME})

# Add dependencies in vendor/CMakeLists.txt
# Header only dependencies are added via target_include_dirs
add_subdirectory(vendor)

file(GLOB_RECURSE "KRAKEN_LIB_SOURCE" "${CMAKE_SOURCE_DIR}/KRAKEN/src/*.cpp")
file(GLOB_RECURSE "KRAKEN_LIB_HEADER" "${CMAKE_SOURCE_DIR}/KRAKEN/include/*.hpp")

# Group files together. May look better in some IDEs
foreach(item IN ITEMS "${KRAKEN_LIB_SOURCE}" "${KRAKEN_LIB_HEADER}")
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

# Include Libraries to Krakenlib
target_link_libraries(${KRAKEN_LIB_NAME} PUBLIC glfw Vulkan::Vulkan)
target_include_directories(${KRAKEN_LIB_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/KRAKEN/include/" "vendor/glm/" )

# Compile Options
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(${KRAKEN_LIB_NAME} PRIVATE "/W4" "/MP" "/std:c++17" "/WX")
endif()
