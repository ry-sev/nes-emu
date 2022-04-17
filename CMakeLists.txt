cmake_minimum_required(VERSION 3.16)

project(
    nes-emu
    VERSION 1.0
    DESCRIPTION "NES Emulator"
    LANGUAGES C CXX
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INSTALL_MESSAGE NEVER)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(IMGUI_DIR external/imgui)
set(SRC_DIR src)

add_compile_options(-Wduplicated-cond)
add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wunused)
add_compile_options(-fdiagnostics-color=always)
add_compile_options(-fstack-clash-protection)
add_compile_options(-fstack-protector-strong)
add_compile_options(-g3)

add_subdirectory(${IMGUI_DIR})

# glfw
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
link_libraries(${GLFW_LIBRARY_DIRS})

# opengl
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

include_directories(${IMGUI_DIR})
include_directories(include)
include_directories(src)

set(SOURCE
    ${SRC_DIR}/Bus.cpp
    ${SRC_DIR}/CPU.cpp
    ${SRC_DIR}/GUI/WindowManager.cpp
    ${SRC_DIR}/GUI/Widget.cpp
    ${SRC_DIR}/GUI/RamWidget.cpp
    ${SRC_DIR}/GUI/StatusWidget.cpp
    ${SRC_DIR}/GUI/MenuWidget.cpp
    ${SRC_DIR}/GUI/InstructionWidget.cpp
    ${SRC_DIR}/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} imgui)
target_link_libraries(${PROJECT_NAME} glfw ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})

target_precompile_headers(nes-emu PRIVATE include/util.h)
target_precompile_headers(nes-emu PRIVATE include/types.h)
