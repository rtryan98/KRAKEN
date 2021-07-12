# Copyright 2021 Robert Ryan. See LICENCE.md.

file(GLOB_RECURSE "YGG_ENGINE_SRC" "${CMAKE_SOURCE_DIR}/Yggdrasil/src/*.h" "${CMAKE_SOURCE_DIR}/Yggdrasil/src/*.cpp")
ygg_group_files("${YGG_ENGINE_SRC}")

add_library(${YGG_ENGINE} ${YGG_ENGINE_SRC})
ygg_set_dirs(${YGG_ENGINE})

# Add Precompiled Header
if (MSVC)
    set_target_properties(${YGG_ENGINE} PROPERTIES COMPILE_FLAGS "/YuYggdrasil/pch.h")
    set_source_files_properties(${CMAKE_SOURCE_DIR}/Yggdrasil/src/Yggdrasil/pch.cpp PROPERTIES COMPILE_FLAGS "/YcYggdrasil/pch.h")
endif (MSVC)

target_include_directories(${YGG_ENGINE} PUBLIC "${CMAKE_SOURCE_DIR}/Yggdrasil/src")
target_include_directories(${YGG_ENGINE} PRIVATE "${CMAKE_SOURCE_DIR}/vendor")
target_link_libraries(${YGG_ENGINE} PUBLIC spdlog)
target_link_libraries(${YGG_ENGINE} PUBLIC Vulkan::Vulkan)
target_link_libraries(${YGG_ENGINE} PUBLIC glslang)
target_link_libraries(${YGG_ENGINE} PUBLIC SPIRV)
target_link_libraries(${YGG_ENGINE} PUBLIC spirv-reflect)
# target_link_libraries(${YGG_ENGINE} PUBLIC "$ENV{VULKAN_SDK}/Lib/spirv-cross-core.lib") # It works on windows but it should change per platform.

if(MSVC)
    target_compile_options(${YGG_ENGINE} PRIVATE "/W4" "/MP" "/std:c++17" "/WX" "/wd26812")
endif()
