set(YGG_ENGINE "Yggdrasil")

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
