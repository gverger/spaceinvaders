cmake_minimum_required(VERSION 3.15)
project(space-invaders)

set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")

add_subdirectory(graphics)
add_subdirectory(logic)
add_subdirectory(src)
