cmake_minimum_required(VERSION 3.1)

project(graipe)

include(../../config/common.cmake)

add_subdirectory(gui)
add_subdirectory(client)
add_subdirectory(server)
