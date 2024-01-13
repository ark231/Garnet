cmake_minimum_required(VERSION 3.28.1)

project(
    WomuYuroCompilerLibAst
    VERSION 0.1
    LANGUAGES CXX)

add_library(
    ast STATIC
    # cmake-format: off
    # lib src begin
    unit.cpp
    # lib src end
    # cmake-format: on
    #
)
target_include_directories(ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
