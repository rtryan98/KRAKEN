# Copyright 2021 Robert Ryan. See LICENCE.md.

file(GLOB_RECURSE "YGG_SANDBOX_SRC" "${CMAKE_SOURCE_DIR}/Sandbox/src/*.h" "${CMAKE_SOURCE_DIR}/Sandbox/src/*.cpp")
ygg_group_files("${YGG_SANDBOX_SRC}")

add_executable(${YGG_SANDBOX} ${YGG_SANDBOX_SRC})
ygg_set_dirs(${YGG_SANDBOX})

target_link_libraries(${YGG_SANDBOX} PRIVATE ${YGG_ENGINE})

if(MSVC)
    target_compile_options(${YGG_SANDBOX} PRIVATE "/W4" "/MP" "/std:c++17" "/WX" "/wd26812")
endif()
