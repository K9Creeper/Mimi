add_library(Mimi
    mimi.c
    cpu.c
    bus.c

    bus-dev/mem.c
    bus-dev/rom.c
)

target_include_directories(Mimi
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
