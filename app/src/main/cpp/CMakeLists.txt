cmake_minimum_required(VERSION 3.18.1)

project("openglbenchmark")

add_library(
        openglbenchmark

        SHARED

        native-lib.cpp
)

target_compile_definitions(openglbenchmark PRIVATE BX_CONFIG_DEBUG=1)

add_subdirectory(glad)

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
)