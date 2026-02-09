cmake_minimum_required(VERSION 3.14)
project(Chip8Emulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(chip8 
    src/main.cpp
    src/Chip8.cpp
    src/Platform.cpp
)

target_link_libraries(chip8 PRIVATE SDL2::SDL2)