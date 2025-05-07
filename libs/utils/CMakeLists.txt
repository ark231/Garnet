cmake_minimum_required(VERSION 3.28.1)

project(
    GarnetLibUtils
    VERSION 0.1
    LANGUAGES CXX)

add_library(
    utils STATIC
    # cmake-format: off
    # lib src begin
    mudig_converter.cpp
    format_support.cpp
    # lib src end
    # cmake-format: on
    #
)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(utils PRIVATE $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra> $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra >
                                     $<$<CXX_COMPILER_ID:MSVC>:/W4>)
target_compile_features(utils PUBLIC cxx_std_20)
