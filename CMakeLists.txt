cmake_minimum_required(VERSION 3.14)
project(Chip8Emulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(chip8 
    main.cpp
    Chip8.cpp
    Platform.cpp
)

target_link_libraries(chip8 ${SDL2_LIBRARIES})