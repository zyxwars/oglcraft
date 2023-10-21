cmake_minimum_required(VERSION 3.0.0)
project(oglc VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

set(GLFW_INCLUDE_DIR "./dependencies/glfw-3.3.8.bin.WIN32/include")
set(GLFW_LINK_DIR "./dependencies/glfw-3.3.8.bin.WIN32/lib-vc2022")

set(GLEW_INCLUDE_DIR "./dependencies/glew-2.1.0/include")
set(GLEW_LINK_DIR "./dependencies/glew-2.1.0/lib/Release/Win32")

set(FREETYPE_INCLUDE_DIR "./dependencies/FreeType/include")
set(FREETYPE_LINK_DIR "./dependencies/FreeType/release static/vs2015-2022/win32")

add_executable(oglc ./src/main.c ./src/Shader.c ./src/GlWrapper.c ./src/Chunk.c ./src/Block.c ./src/Camera.c)

target_include_directories(oglc PUBLIC src includes ${GLFW_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR})
target_link_directories(oglc PUBLIC ${GLFW_LINK_DIR} ${GLEW_LINK_DIR} ${FREETYPE_LINK_DIR})
target_link_libraries(oglc glfw3.lib opengl32.lib glew32s.lib glu32.lib freetype.lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
