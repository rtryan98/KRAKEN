# Third Party library
file(GLOB_RECURSE "THIRD_PARTY_SOURCE" "${CMAKE_SOURCE_DIR}/ThirdParty/src/*.cpp")

yggdrasil_group_files(${THIRD_PARTY_SOURCE})

add_library(Yggdrasil-ThirdParty ${THIRD_PARTY_SOURCE})
target_include_directories(Yggdrasil-ThirdParty PRIVATE "${CMAKE_SOURCE_DIR}/vendor/"
                                                        "${CMAKE_SOURCE_DIR}/vendor/imgui")
target_link_libraries(Yggdrasil-ThirdParty PRIVATE Vulkan::Vulkan glfw)

# Compile Options
if(MSVC)
    target_compile_options(Yggdrasil-ThirdParty PRIVATE "/w" "/MP" "/std:c++17")
endif()

yggdrasil_set_dirs(Yggdrasil-ThirdParty)
