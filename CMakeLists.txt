cmake_minimum_required(VERSION 2.8)


project(shadow CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(main src/main.cpp)
target_include_directories(main
    PRIVATE include/
    )
set_target_properties(main PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    CXX_STANDARD 11
    )

