cmake_minimum_required(VERSION 3.28.1)

project(
    WomuYuroCompiler
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_library(reflex_lib reflex REQUIRED)
find_package(
    ICU
    COMPONENTS uc
    REQUIRED)
find_package(fmt REQUIRED)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/bison)
make_directory(${CMAKE_CURRENT_BINARY_DIR}/reflex)

add_custom_target(Scanner)
add_custom_command(
    TARGET Scanner
    OUTPUTS ${CMAKE_CURRENT_BINARY_DIR}/reflex/scanner.cpp DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scanner.ll
    COMMAND reflex-lexer ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/reflex/scanner.cpp
            --header-file=${CMAKE_CURRENT_BINARY_DIR}/reflex/scanner.hpp ${CMAKE_CURRENT_SOURCE_DIR}/scanner.ll)
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

# Build target
add_executable(WomuYuroCompiler main.cpp driver.cpp ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS} mudig_converter.cpp)

target_include_directories(WomuYuroCompiler PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/bison ${CMAKE_CURRENT_BINARY_DIR}/reflex)
target_include_directories(WomuYuroCompiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(WomuYuroCompiler Scanner)
target_link_libraries(WomuYuroCompiler ${reflex_lib} ICU::uc fmt::fmt)

# include(cmake/CPM.cmake)

# cpmaddpackage()

# target_link_libraries()

target_compile_options(WomuYuroCompiler PRIVATE $<$<CXX_COMPILER_ID:Clang>:-Wall -Weverything>
                                                $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra > $<$<CXX_COMPILER_ID:MSVC>:/W4>)
