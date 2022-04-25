cmake_minimum_required(VERSION 3.18.1)

project("openglbenchmark")

add_library(
        openglbenchmark

        SHARED

        opengl-renderer.cpp
)

target_compile_definitions(openglbenchmark PRIVATE BX_CONFIG_DEBUG=1)
target_compile_definitions(openglbenchmark PRIVATE STB_IMAGE_IMPLEMENTATION)

add_subdirectory(glad)

include_directories(
        glm
        stb_image
        assimp/include
        assimp/build/include/
)

find_library(
        log-lib

        log
)

target_link_libraries(
        openglbenchmark

        android
        ${log-lib}
        GLESv3
        EGL
        glad
        ${CMAKE_CURRENT_SOURCE_DIR}/assimp/build/bin/libassimp.so
)