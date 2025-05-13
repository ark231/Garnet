cmake_minimum_required(VERSION 3.28.1)

project(
    GarnetLibAst
    VERSION 0.1
    LANGUAGES CXX)

#cmake_policy(SET CMP0167 NEW)
find_package(Boost REQUIRED)
find_package(magic_enum REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
CPMAddPackage("gh:fmtlib/fmt#11.2.0")

add_library(
    ast STATIC
    # cmake-format: off
    # lib src begin
    compilation_unit.cpp
    concrete_source_identifiers.cpp
    concrete_decls.cpp
    concrete_expressions.cpp
    concrete_statements.cpp
    concrete_infos.cpp
    concrete_defs.cpp
    block.cpp
    # lib src end
    # cmake-format: on
    #
    pretty_printer/pretty_printer.cpp)
target_include_directories(ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})
target_link_libraries(ast PUBLIC defs utils magic_enum fmt::fmt)
target_compile_options(ast PRIVATE $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra> $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra >
                                   $<$<CXX_COMPILER_ID:MSVC>:/W4>)
