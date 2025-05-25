cmake_minimum_required(VERSION 3.28)

project(
    Garnet
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/CPM.cmake)
cpmaddpackage(NAME magic_enum GITHUB_REPOSITORY Neargye/magic_enum GIT_TAG v0.9.5)
cpmaddpackage("gh:fmtlib/fmt#11.2.0")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
# find_library(reflex_lib reflex REQUIRED) find_package(Reflex REQUIRED)
cpmaddpackage("gh:Genivia/RE-flex@5.5.0")
find_package(
    ICU
    COMPONENTS uc
    REQUIRED)
# find_package(fmt REQUIRED)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

find_package(
    Boost 1.83
    COMPONENTS program_options
    REQUIRED)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/bison)
make_directory(${CMAKE_CURRENT_BINARY_DIR}/reflex)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/reflex/scanner.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scanner.ll
    DEPENDS Reflex
    COMMAND Reflex ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/reflex/scanner.cpp
            --header-file=${CMAKE_CURRENT_BINARY_DIR}/reflex/scanner.hpp ${CMAKE_CURRENT_SOURCE_DIR}/scanner.ll)
add_custom_target(Scanner SOURCES ${CMAKE_CURRENT_BINARY_DIR}/reflex/scanner.cpp)
set(FLEX_Scanner_OUTPUTS ${CMAKE_CURRENT_BINARY_DIR}/reflex/scanner.cpp)
set_property(
    TARGET Scanner
    APPEND
    PROPERTY ADDITIONAL_CLEAN_FILES
             ${CMAKE_CURRENT_BINARY_DIR}/reflex/scanner.cpp;${CMAKE_CURRENT_BINARY_DIR}/reflex/scanner.hpp)

bison_target(Parser parser.yy ${CMAKE_CURRENT_BINARY_DIR}/bison/parser.cpp
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/bison/parser.hpp)
set_property(
    DIRECTORY
    APPEND
    PROPERTY ADDITIONAL_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/bison/location.hh)

add_flex_bison_dependency(Scanner Parser)

add_subdirectory(libs)

# Build target
add_executable(
    Garnet
    # cmake-format: off
    # exe src start
    main.cpp
    driver.cpp
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Scanner_OUTPUTS}
    # exe src end
    # cmake-format: on
    #
)

target_include_directories(Garnet PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/bison ${CMAKE_CURRENT_BINARY_DIR}/reflex)
target_include_directories(Garnet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Garnet PUBLIC ${magic_enum_SOURCE_DIR}/include)
add_dependencies(Garnet Scanner)
target_link_libraries(
    Garnet
    ReflexLib
    ICU::uc
    fmt::fmt
    Boost::program_options
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,14.0>>:
    stdc++_libbacktrace
    >
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,14.0>>:
    stdc++exp
    >
    ast
    defs
    utils)

target_compile_definitions(Garnet PRIVATE BOOST_STACKTRACE_USE_ADDR2LINE BOOST_STACKTRACE_USE_BACKTRACE)

# cpmaddpackage()

# target_link_libraries()

target_compile_options(Garnet PRIVATE $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra> $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra
                                      > $<$<CXX_COMPILER_ID:MSVC>:/W4>)
add_library(interpreter interpreter/interpreter.cpp)
target_link_libraries(
    interpreter
    ast
    defs
    utils
    fmt::fmt
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,14.0>>:
    stdc++_libbacktrace
    >
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,14.0>>:
    stdc++exp
    >)

target_link_libraries(Garnet interpreter)

target_compile_options(interpreter PRIVATE $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra> $<$<CXX_COMPILER_ID:GNU>:-Wall
                                           -Wextra > $<$<CXX_COMPILER_ID:MSVC>:/W4>)

# # for debugging message(STATUS "*** dump start cmake variables ***") get_cmake_property(_variableNames VARIABLES)
# foreach(_variableName ${_variableNames}) message(STATUS "${_variableName}=${${_variableName}}") endforeach()
# message(STATUS "*** dump end ***")
