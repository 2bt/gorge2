cmake_minimum_required(VERSION 3.7)

project(gorge)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

find_package(PkgConfig REQUIRED)

pkg_search_module(GLEW REQUIRED)
pkg_search_module(SDL2IMAGE REQUIRED)

link_libraries(${GLEW_LIBRARIES})
link_libraries(${SDL2IMAGE_LIBRARIES})

include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${SDL2IMAGE_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}
		src/background.cpp
		src/enemy.cpp
		src/fx.cpp
		src/gfx.cpp
		src/main.cpp
		src/particle.cpp
		src/player.cpp
		src/sprite.cpp
		src/sprite_renderer.cpp
		src/square_enemy.cpp
		src/util.cpp
		src/wall.cpp
		src/world.cpp
		)
