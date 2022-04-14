cmake_minimum_required(VERSION 3.16)

project(
    nes-emu
    VERSION 1.0
    DESCRIPTION "NES Emulator"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

set(CMAKE_INSTALL_MESSAGE NEVER)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wduplicated-cond)
add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wunused)
add_compile_options(-fdiagnostics-color=always)
add_compile_options(-fstack-clash-protection)
add_compile_options(-fstack-protector-strong)
add_compile_options(-g3)

include_directories(include)
add_subdirectory(src)

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(nes-emu ${SOURCES})
target_precompile_headers(nes-emu PRIVATE include/util.h)
target_precompile_headers(nes-emu PRIVATE include/types.h)
