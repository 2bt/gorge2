cmake_minimum_required(VERSION 3.7)

project(gorge)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

find_package(PkgConfig REQUIRED)

pkg_search_module(GLEW REQUIRED glew)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image)

link_libraries(${GLEW_LIBRARIES})
link_libraries(${SDL2IMAGE_LIBRARIES})

include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${SDL2IMAGE_INCLUDE_DIRS})

file(GLOB SRC "src/*.hpp" "src/*.cpp")

add_executable(${PROJECT_NAME} ${SRC})
