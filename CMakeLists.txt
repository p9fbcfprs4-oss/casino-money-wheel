cmake_minimum_required(VERSION 3.10)
project(CasinoMoneyWheel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_gfx REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

# Add the executable
add_executable(casino_wheel 
    src/main.cpp
    src/wheel.cpp
)

# Link libraries
target_link_libraries(casino_wheel 
    ${SDL2_LIBRARIES}
    SDL2_gfx
    m
)
