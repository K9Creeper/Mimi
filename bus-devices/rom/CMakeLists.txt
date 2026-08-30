add_library(rom OBJECT
    src/rom.c
)

target_include_directories(rom
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        include
)