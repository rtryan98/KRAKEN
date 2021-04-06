cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/scripts")
include(Yggdrasil)

project(Yggdrasil)
add_subdirectory("${CMAKE_SOURCE_DIR}/Yggdrasil")
