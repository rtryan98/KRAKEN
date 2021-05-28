# Copyright 2021 Robert Ryan. See LICENCE.md.

cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/scripts")
include(Yggdrasil)

set(YGG_ENGINE "Yggdrasil")
set(YGG_SANDBOX "Yggdrasil-Sandbox")

project(Yggdrasil)
find_package(Vulkan REQUIRED)
add_subdirectory("${CMAKE_SOURCE_DIR}/vendor")
add_subdirectory("${CMAKE_SOURCE_DIR}/Yggdrasil")
add_subdirectory("${CMAKE_SOURCE_DIR}/Sandbox")

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${YGG_SANDBOX})
endif()
