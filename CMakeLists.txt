cmake_minimum_required(VERSION 3.13)

project(kash)

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

set(CMAKE_C_STANDARD 23) # Enable the C23 standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(kash ${SOURCE_FILES})

include_directories(${CMAKE_SOURCE_DIR})

target_link_libraries(kash PRIVATE readline m)
