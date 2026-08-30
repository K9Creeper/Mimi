add_library(mem OBJECT
    src/mem.c
)

target_include_directories(mem
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        include
)