add_library(Mimi
    mimi.c
    cpu.c
    bus.c
)

target_include_directories(Mimi
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
