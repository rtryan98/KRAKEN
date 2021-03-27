# Third Party library
file(GLOB_RECURSE "THIRD_PARTY_SOURCE" "${CMAKE_SOURCE_DIR}/ThirdParty/src/*.cpp")

foreach(item IN ITEMS ${THIRD_PARTY_SOURCE})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "/" "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

add_library(Yggdrasil-ThirdParty ${THIRD_PARTY_SOURCE})
target_include_directories(Yggdrasil-ThirdParty PRIVATE "${CMAKE_SOURCE_DIR}/vendor/"
                                                        "${CMAKE_SOURCE_DIR}/vendor/imgui")
target_link_libraries(Yggdrasil-ThirdParty PRIVATE Vulkan::Vulkan glfw)

# Compile Options
if(MSVC)
    target_compile_options(Yggdrasil-ThirdParty PRIVATE "/W0" "/MP" "/std:c++17")
endif()

# Output Directories
set_target_properties(Yggdrasil-ThirdParty PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/debug/")
set_target_properties(Yggdrasil-ThirdParty PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/relwithdebinfo/")
set_target_properties(Yggdrasil-ThirdParty PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/minsizerel/")
set_target_properties(Yggdrasil-ThirdParty PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/${ENGINE_PROJECT_NAME}/release/")
