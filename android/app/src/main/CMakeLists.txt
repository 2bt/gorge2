cmake_minimum_required(VERSION 3.4.1)

set(LIB_DIR "$ENV{HOME}/Programming/android/lib")

project(main)

add_compile_options(-std=c++17 -O3 -Wall)

set(LIBTYPE STATIC)
set(ALSOFT_INSTALL OFF)
set(ALSOFT_REQUIRE_OBOE ON)
set(OBOE_SOURCE "${LIB_DIR}/oboe")
add_subdirectory("${LIB_DIR}/openal-soft" ./openal)

file(GLOB SRC
    "cpp/*.hpp"
    "cpp/*.cpp"
    )

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
    "${LIB_DIR}/glm"
    "${LIB_DIR}/openal-soft/include"
    )

target_link_libraries(${PROJECT_NAME}
    android
    log
    EGL
    GLESv2
    atomic
    OpenAL
    )

