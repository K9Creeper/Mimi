cmake_minimum_required(VERSION 3.8)

project(Mimi
    VERSION 1.0
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_PROFILE 6502 CACHE STRING "Target CPU profile")

add_subdirectory(src)
add_subdirectory(profiles/${TARGET_PROFILE})

add_subdirectory(tests)
