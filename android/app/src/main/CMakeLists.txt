cmake_minimum_required(VERSION 3.4.1)

set(LIB_DIR "$ENV{HOME}/Programming/android/lib")

project(main)

add_compile_options(-std=c++17 -O3 -Wall)

file(GLOB SRC
    "cpp/*.hpp"
    "cpp/*.cpp"
    )

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
    "${LIB_DIR}/glm"
    )

target_link_libraries(${PROJECT_NAME}
    android
    log
    EGL
    GLESv2
    atomic
    )

