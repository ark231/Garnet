cmake_minimum_required(VERSION 3.28.1)

project(
    WomuYuroCompilerLibUtils
    VERSION 0.1
    LANGUAGES CXX)

add_library(
    utils STATIC
    # cmake-format: off
    # lib src begin
    mudig_converter.cpp
    # lib src end
    # cmake-format: on
    #
)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
