cmake_minimum_required(VERSION 3.28.1)

project(
    GarnetLibDefs
    VERSION 0.1
    LANGUAGES CXX)

add_library(defs INTERFACE)

target_include_directories(defs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
